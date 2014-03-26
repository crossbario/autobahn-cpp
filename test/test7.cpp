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



int main () {

   // A Worker MUST log to std::cerr, since std::cin/cout is used
   // for talking WAMP with the master
   //
   cerr << "Worker starting .." << endl;

   // Setup WAMP session running over stdio
   //
   autobahn::session session(cin, cout);


   auto s = session.join(std::string("realm1")).then([&session](future<int> s) {

      // WAMP session is now established ..
      //
      cerr << "Joined with session ID " << s.get() << endl;


      // Publish an event
      //
      session.publish("com.myapp.topic1", {1, string("zwei"), false}, {{"foo", 23}, {"bar", string("baz")}});


      // Issue some remote procedure calls ..
      //

      auto call1 = session.call("com.math.slowsquare", {3}).then([](future<any> f) {

         uint64_t res = any_cast<uint64_t> (f.get());
         cerr << "Got result 1: " << res << endl;
      });


      auto call2 = session.call("com.mathservice.add2", {23, 777})
         .then([](future<any> f) {

            uint64_t res = any_cast<uint64_t> (f.get());
            cerr << "Got result 2: " << res << endl;
            return res;
         })
         .then([](future<uint64_t> f) {

            uint64_t res = f.get();
            cerr << "Got result 2b: " << res << endl;
         })
      ;

      auto call3 = session.call("com.mathservice.add2", {23, 7}).then([&session](future<any> f) {

         uint64_t res = any_cast<uint64_t> (f.get());

         stringstream s;
         s << "Got result 3: " << res;

         cerr << s.str() << endl;

         // Publish an event
         //
         session.publish("com.myapp.topic1", {string("Event from C++")}, {{"msg", s.str()}});

         // and issue another RPC ..
         //
         auto call4 = session.call("com.math.slowsquare", {res}, {{"delay", 3}}).then([](future<any> f) {

            uint64_t res = any_cast<uint64_t> (f.get());
            cerr << "Got result 4: " << res << endl;
         });

         // Wait for call4 to finish before returning ..
         //
         call4.get();
      });


      // Wait for all calls to finish ..
      //
      when_all(std::move(call1), std::move(call2), std::move(call3)).get();
      cerr << "Done." << endl;

      // Stop the event loop and exit the program.
      //
      session.stop();
   });

   // Enter event loop for session. This will not return ..
   //
   session.loop();
}
