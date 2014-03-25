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

#include "autobahn.hpp"


int main () {

   // A Worker MUST log to std::cerr, since std::cin/cout is used
   // for talking WAMP with the master
   //
   std::cerr << "Worker starting .." << std::endl;

   // Setup WAMP session running over stdio
   //
   autobahn::session session(std::cin, std::cout);

   // Launch policy to use
   //
   //boost::launch lp = boost::launch::deferred;

   // To establish a session, we join a "realm" ..
   //
   session.join(std::string("realm1")).then([&](boost::future<int> f) {

      // WAMP session is now established ..
      //
      int session_id = f.get();

      std::cerr << "Joined with session ID " << session_id << std::endl;

      // call a remote procedure ..
      //
      session.call("com.mathservice.add2", 23, 777).then([](boost::future<boost::any> f) {

         // call result received
         //
         int res = boost::any_cast<int> (f.get());
         std::cerr << "Got RPC result " << res << std::endl;
      });
   });

   // Enter event loop for session ..
   //
   session.loop();
}
