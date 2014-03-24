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

boost::any add2(autobahn::anyvec& args) {
   std::cout << "I am being called" << std::endl;
   int a = boost::any_cast<int>(args[0]);
   int b = boost::any_cast<int>(args[1]);
   return a + b;
}


typedef boost::any (*callback) (autobahn::anyvec&);

void registerproc(const std::string& procedure, callback endpoint) {
   std::cout << "registering procedure" << std::endl;
}


#include <unistd.h>


int main () {

   std::cerr << "C++ worker starting .." << std::endl;
   std::cerr.flush();

   // WAMP session running over stdio
   //
   autobahn::session session(std::cin, std::cout);

#if 0
   boost::future<int> f = session.join(std::string("realm1"));

   std::cout.flush();
   close(STDOUT_FILENO);

   session.loop();
   sleep(2);

#else
   session.join(std::string("realm1")).then([&](boost::future<int> f) {


      int session_id = 44;
      try {
         std::cerr << "JOINING .." << std::endl;
         std::cerr << f.valid() << std::endl;
         //std::cerr << f.is_ready() << std::endl;
         //std::cerr << f.has_value() << std::endl;
         //std::cerr << f.has_exception() << std::endl;
         std::cerr << "JOINING 2 .." << std::endl;
         std::cerr.flush();
         session_id = f.get();
      }
      catch (...) {
         std::cerr << "Unknown problem" << std::endl;
         std::cerr.flush();
      }


      std::cerr << "session id: " << session_id << std::endl;
      std::cerr.flush();
   });

   std::cout.flush();
   close(STDOUT_FILENO);

   session.loop();
   sleep(2);
#endif
/*
   session.join(std::string("realm1")).then([&](boost::future<int> res) {

      std::cerr << "session id: " << res.get() << std::endl;
   });

   session.loop();
*/

   //session.registerproc("com.mathservice.add2", &add2);
/*

   autobahn::anyvec args;
   args.push_back(23);
   args.push_back(7);

#if 1
   boost::future<boost::any> f = session.call("com.mathservice.add2", args);
   std::cout.flush();
   close(STDOUT_FILENO);

   boost::any res = f.get();
#else
   session.call("com.mathservice.add2", args).then([](boost::future<boost::any> f) {
      int res = boost::any_cast<int> (f.get());
      std::cerr << "got result: " << res << std::endl;
   });
#endif
*/
   //std::cout.close();

   //sleep(2);




/*
   boost::any res = session.invoke("com.mathservice.add2", args);

   std::cout << boost::any_cast<int>(res) << std::endl;

   session.join(std::string("realm1")).then([&](boost::future<int> res) {

      std::cerr << "session id: " << res.get() << std::endl;

      // event without any payload
      //
      session.publish("com.myapp.topic1");

   });
*/
}
