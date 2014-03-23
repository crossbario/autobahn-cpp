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

   // WAMP session running over stdio
   //
   autobahn::session session(std::cin, std::cout);


   // event without any payload
   //
   session.publish("com.myapp.topic1");


   // event with positional payload
   //
   session.publish("com.myapp.topic1", 23, true, std::string("hello"));


   // event with complex positional payload
   //
   autobahn::anyvec v;
   v.push_back(1);
   v.push_back(3.123);
   v.push_back(false);
   v.push_back(std::string("hello"));

   autobahn::anyvec v2;
   v2.push_back(std::string("foo"));
   v2.push_back(std::string("bar"));

   v.push_back(v2);

   autobahn::anymap m;
   m["foo"] = 23;
   m["bar"] = 1.23;
   m["baz"] = std::string("awesome");

   v.push_back(m);

   session.publish("com.myapp.topic1", v);


   // event with keyword payload
   //
   autobahn::anymap m2;
   m2["a"] = 23;
   m2["b"] = std::string("foobar");

   session.publish("com.myapp.topic1", m2);


   // event with position and keyword payload
   //
   session.publish("com.myapp.topic1", v2, m2);
}
