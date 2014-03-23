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

#include <msgpack.hpp>
#include <iostream>
#include <string>
#include <map>

#include "autobahn.hpp"

struct PersonDetail {
   std::string forename;
   int age;

   MSGPACK_DEFINE(forename, age);
};

struct Person {
   std::string name;
   std::vector<int> points;
   std::map<std::string, int> votes;

   PersonDetail detail;

   MSGPACK_DEFINE(name, points, votes, detail);
};


#include <map>
#include <boost/any.hpp>

using boost::any_cast;



int main () {
   Person person;
   person.name = "sdfsdfs";
   person.points.push_back(3);
   person.points.push_back(4);
   person.points.push_back(5);
   person.votes["jg"] = 99;
   person.votes["absds"] = 23;
   person.detail.age = 23;
   person.detail.forename = "joe";

   msgpack::sbuffer buffer;
   msgpack::packer<msgpack::sbuffer> packer(&buffer);

   //packer.pack(person);

   packer.pack_array(4);
   packer.pack(23);
   packer.pack(std::string("sdfsfd"));
   packer.pack(person);

   std::map<std::string, msgpack::object> o;
   o["a"] = 23;
   o["b"] = true;
   o["c"] = std::string("hello");

   std::vector<int> v;
   v.push_back(1);
   v.push_back(2);
   v.push_back(3);
   //o["d"] = msgpack::object(v);

   packer.pack(o);

   //std::cout.write(buffer.data(), buffer.size());

   autobahn::anymap map;

   map["a"] = 2;
   map["b"] = std::string("sdfsdfs");   
   map["c"] = v;
   //std::cerr << map << std::endl;

   autobahn::anyvec v2;
   v2.push_back(23);
   v2.push_back(23.23424);
   v2.push_back(std::string("sdfsf"));
   map["d"] = v2;

   std::cout << map["a"].type().name() << std::endl;
   std::cout << map["b"].type().name() << std::endl;
   std::cout << map["c"].type().name() << std::endl;
   std::cout << map["d"].type().name() << std::endl;
   //std::cout << map["a"].type().hash_code() << std::endl;
   std::cout << (map["a"].type() == typeid(int)) << std::endl;

   autobahn::anyvec v3;
   v3.push_back(v2);
   v3.push_back(23);

   //WampSession session(std::cin, std::cout);
   //session.publish("com.myapp.topic1", v3);
   //session.pack_any(v3);
}
