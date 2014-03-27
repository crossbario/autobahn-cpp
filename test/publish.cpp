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



int main () {

   // A Worker MUST log to std::cerr, since std::cin/cout is used
   // for talking WAMP with the master
   //
   cerr << "Worker starting .." << endl;


   // Setup WAMP sess running over stdio
   //
   autobahn::session sess(cin, cout);


   auto s = sess.join(std::string("realm1"));

   auto s2 = s.then([&](decltype(s) f) {

      // WAMP session is now established ..
      //
      cerr << "Joined with session ID " << f.get() << endl;

      sess.publish("com.myapp.topic1", {string("Hello, world!")});

      // Stop the event loop and exit the program.
      //
      //sess.stop();
   });

   //s2.get();

   // Enter event loop for sess. This will not return ..
   //
   sess.loop();
}
