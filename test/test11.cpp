///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2014 Tavendo GmbH
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <sstream>

#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include "autobahn.hpp"

using namespace std;
using namespace boost;
using namespace autobahn;


any add2(const anyvec& args, const anymap& kwargs) {

   cerr << "I am being called" << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);
   return x + y;
}


anyvec add2b(const anyvec& args, const anymap& kwargs) {

   cerr << "I am being called" << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);
   return {x + y, x * 2};
}

anyvec add2c(const anyvec& args, const anymap& kwargs) {

   cerr << "I am being called" << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);
   return {x + y, x};
}


future<anyvecmap> add2d(const anyvec& args, const anymap& kwargs) {

   cerr << "I am being called" << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);
   return make_ready_future(std::make_pair(anyvec({x + y, 2 * x}), anymap({{"foo", 23}})));
}


autobahn::session* gsess;


future<anyvecmap> add2e(const anyvec& args, const anymap& kwargs) {

   cerr << "I am being called" << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);

   promise<anyvecmap> p;


   // and issue another RPC ..
   //
   auto call4 = gsess->call("com.math.slowsquare", {x + y}, {{"delay", 1}});

   auto call4f = call4.then([&](decltype(call4) f) {

      cerr << "GOT RESULT 44" << endl;

      uint64_t res = any_cast<uint64_t> (f.get());
      cerr << "Got result 4: " << res << endl;

      p.set_value(std::make_pair(anyvec({x + y, 2 * x, res}), anymap({{"foo", 23}})));
//      return std::make_pair(anyvec({x + y, 2 * x, res}), anymap({{"foo", 23}}));
//      return make_ready_future(std::make_pair(anyvec({x + y, 2 * x, res}), anymap({{"foo", 23}})));
   });

   // Wait for call4 to finish before returning ..
   //
   cerr << "HERE 55" << endl;
   return p.get_future();
/*
   return call4;
   call4.get();
   cerr << "HERE 66" << endl;
   return make_ready_future(std::make_pair(anyvec(), anymap()));
*/
}



int main () {

   // A Worker MUST log to std::cerr, since std::cin/cout is used
   // for talking WAMP with the master
   //
   cerr << "Worker starting .." << endl;

   auto foo = make_ready_future(23);

   // Setup WAMP sess running over stdio
   //
   autobahn::session sess(cin, cout);
   //sess = autobahn::session(cin, cout);

   gsess = &sess;


   auto s = sess.join(std::string("realm1")).then([&sess](future<int> s) {

      // WAMP session is now established ..
      //
      cerr << "Joined with session ID " << s.get() << endl;

      auto call4 = sess.call("com.math.slowsquare", {55}, {{"delay", 1}}).then([&](future<any> f) {

         uint64_t res = any_cast<uint64_t> (f.get());
         cerr << "Got result 4: " << res << endl;

      });

      call4.get();

#if 0
#if 1
      auto r = sess.provide("com.myapp.cpp.add2", &add2);
#else
      auto r = sess.provide("com.myapp.cpp.add2",

         [](const anyvec& args, const anymap& kwargs) {

            uint64_t x = any_cast<uint64_t> (args[0]);
            uint64_t y = any_cast<uint64_t> (args[1]);
            return x + y;
         }
      );
#endif

#else

#if 1
//      auto r = sess.provide_v("com.myapp.cpp.add2", &add2b);
//      auto r = sess.providef_vm("com.myapp.cpp.add2", &add2d);
      auto r = sess.providef_vm("com.myapp.cpp.add2", &add2e);
#else
      auto r = sess.provide("com.myapp.cpp.add2",

         [](const anyvec& args, const anymap& kwargs) {

            uint64_t x = any_cast<uint64_t> (args[0]);
            uint64_t y = any_cast<uint64_t> (args[1]);
            return x + y;
         }
      );
#endif

#endif

      auto r2 = r.then([&](decltype(r)) {
         cerr << "procedure registered" << endl;
         sess.publish("com.myapp.tryme");
      });

      r2.get();

      // Stop the event loop and exit the program.
      //
      //sess.stop();
   });

   // Enter event loop for sess. This will not return ..
   //
   sess.loop();
}
