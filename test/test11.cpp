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
   return {x + y, x - y};
}

anyvec add2c(const anyvec& args, const anymap& kwargs) {

   cerr << "I am being called" << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);
   return {x + y, x - y};
}

int main () {

   // A Worker MUST log to std::cerr, since std::cin/cout is used
   // for talking WAMP with the master
   //
   cerr << "Worker starting .." << endl;

   any a = &add2b; 
   cerr << "1: " << (a.type() == typeid(endpoint_v_t)) << endl;
   cerr << "1b: " << (a.type() == typeid(endpointf_t)) << endl;

   cerr << "FPtr = " << reinterpret_cast<void*>(any_cast<endpoint_v_t>(a)) << endl;
   cerr << "FPtr = " << reinterpret_cast<void*>(&add2b) << endl;

   a = &add2c;
   cerr << "2: " << (a.type() == typeid(endpoint_v_t)) << endl;
   cerr << "FPtr = " << reinterpret_cast<void*>(any_cast<endpoint_v_t>(a)) << endl;


   // Setup WAMP session running over stdio
   //
   autobahn::session session(cin, cout);


   auto s = session.join(std::string("realm1")).then([&session](future<int> s) {

      // WAMP session is now established ..
      //
      cerr << "Joined with session ID " << s.get() << endl;

      auto r = session.provide("com.myapp.cpp.add2", &add2);

      auto r2 = r.then([&](decltype(r)) {
         cerr << "procedure registered" << endl;
         session.publish("com.myapp.tryme");
      });

      r2.get();

      // Stop the event loop and exit the program.
      //
      //session.stop();
   });

   // Enter event loop for session. This will not return ..
   //
   session.loop();
}
