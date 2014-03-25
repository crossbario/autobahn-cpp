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
/*
   uint16_t x = 18;
   boost::any hx = x;
   std::cerr << "XXX " << boost::any_cast<int>(hx) << std::endl;
*/
   // Setup WAMP session running over stdio
   //
   autobahn::session session(std::cin, std::cout);

   // Launch policy to use
   //
   boost::launch lp = boost::launch::deferred;

   // To establish a session, we join a "realm" ..
   //
   session.join(std::string("realm1")).then([&](boost::future<int> f) {

      // WAMP session is now established ..
      //
      int session_id = f.get();

      std::cerr << "Joined with session ID " << session_id << std::endl;

      autobahn::anyvec params1 = {2, "foobar"};
      autobahn::anymap params2 = {{"a", 23}, {"b", 7}};


      // Variant A: positional args via generic vector, generic result
      //
      autobahn::anyvec args;
      args.push_back(23);
      args.push_back(777);

      session.call("com.mathservice.add2", args).then(lp, [](boost::future<boost::any> f) {
         boost::any a = f.get();
         std::cerr << "::: " << a.type().name() << std::endl;
         std::cerr << "::: " << (a.type() == typeid(uint64_t)) << std::endl;
         uint64_t res = boost::any_cast<uint64_t> (a);
         std::cerr << "A - Got RPC result " << res << std::endl;
      });

#if 0
      // Variant B: generic positional args, generic result
      //
      boost::any a = 23;
      boost::any b = 777;

      session.call("com.mathservice.add2", a, b).then(lp, [](boost::future<boost::any> f) {
         int res = boost::any_cast<int> (f.get());
         std::cerr << "B - Got RPC result " << res << std::endl;
      });


      // Variant C: typed positional args, generic result
      //
      session.call("com.mathservice.add2", 23, 777).then(lp, [](boost::future<boost::any> f) {
         int res = boost::any_cast<int> (f.get());
         std::cerr << "C - Got RPC result " << res << std::endl;
      });

#endif
      // Variant C: typed positional args, generic result
      //
      session.call("com.mathservice.add2", {23, 777}).then(lp, [](boost::future<boost::any> f) {
         int res = boost::any_cast<int> (f.get());
         std::cerr << "D - Got RPC result " << res << std::endl;
      });


      session.call("com.arguments.stars", {}, {{"stars", 10}}).then(lp, [](boost::future<boost::any> f) {
         std::string res = boost::any_cast<std::string> (f.get());
         std::cerr << "E - Got RPC result " << res << std::endl;
      });

/*
      session.call<std::string>("com.arguments.stars", {}, {{"stars", 20}}).then(lp, [](boost::future<std::string> f) {
         std::cerr << "F - Got RPC result " << f.get() << std::endl;
      });
*/

//      session.call("com.arguments.numbers", {1, 7}, {{"prefix", "Hello number: "}}).then(lp, [](boost::future<boost::any> f) {
      session.call("com.arguments.numbers", {1, 7}).then(lp, [](boost::future<boost::any> f) {
         boost::any res = f.get();
         std::cerr << "G - Got RPC result " << res.type().name() << std::endl;

         autobahn::anyvec v = boost::any_cast<autobahn::anyvec>(res);
         std::cerr << "G2 " << v.size() << std::endl;
         for (int i = 0; i < v.size(); ++i) {
            std::cerr << boost::any_cast<std::string>(v[i]) << std::endl;
         }
      });
   });


   // Enter event loop for session ..
   //
   session.loop();
}
