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

#include "autobahn.hpp"


struct Person {
   std::string m_str;
   std::vector<int> m_vec;

   MSGPACK_DEFINE(m_str, m_vec);
};


int main () {
   WampSession session(std::cin, std::cout);

   anyvec v;
   v.push_back(1);
   v.push_back(3.123);
   v.push_back(false);
   v.push_back(std::string("hello"));

   anyvec v2;
   v2.push_back(std::string("foo"));
   v2.push_back(std::string("bar"));

   v.push_back(v2);

   anymap m;
   m["foo"] = 23;
   m["bar"] = 1.23;
   m["baz"] = std::string("awesome");

   v.push_back(m);

   session.publish("com.myapp.topic1", v);

/*   
   session.send_hello("realm2");
   session.send_hello("crossbardemo.realm.test");
   session.send_hello("foobar");

   WampSession::args args;
//   args.push_back("foo");
   args.push_back(msgpack::object(23));
   args.push_back(msgpack::object(0.12345));
   args.push_back(msgpack::object(false));
   args.push_back(msgpack::object(std::string("dfsdfs")));

   session.publish("com.myapp.topic1", args);

   Person person;
   //args.push_back(msgpack::object(person));
   //msgpack::sbuffer sbuf;
   //msgpack::pack(sbuf, person);

   Value value;
   value.add(23);
   value.add(0.12345);
   value.add(false);
   value.add(std::string("sdfsdfs"));
   std::vector<int> v;
   v.push_back(2);
   v.push_back(3);
   v.push_back(4);
   //value.add(v);
   //msgpack::object o(v);

   msgpack::sbuffer sbuf;
   msgpack::pack(sbuf, person);
   msgpack::pack(sbuf, v);
   msgpack::pack(sbuf, value._args);

   //msgpack::object o;
   //o << v;



   //msgpack::object o(person);

   session.publish("com.myapp.topic1", value);
*/
/*   // serializes multiple objects using msgpack::packer.
   msgpack::sbuffer buffer;

   // [HELLO, Realm|uri, Details|dict]

   msgpack::packer<msgpack::sbuffer> pk(&buffer);
   pk.pack(MSG_CODE_HELLO);
   pk.pack(std::string("realm1"));
   pk.pack_map(2);
   pk.pack(std::string("x"));
   pk.pack(3);
   pk.pack(std::string("y"));
   pk.pack(3.4321);

   // deserializes these objects using msgpack::unpacker.
   msgpack::unpacker pac;

   // feeds the buffer.
   pac.reserve_buffer(buffer.size());
   memcpy(pac.buffer(), buffer.data(), buffer.size());
   pac.buffer_consumed(buffer.size());

   // now starts streaming deserialization.
   msgpack::unpacked result;
   while (pac.next(&result)) {
      msgpack::object obj = result.get();
      std::cout << obj.type << std::endl;
      std::cout << obj << std::endl;
   }
*/
   // results:
   // $ g++ stream.cc -lmsgpack -o stream
   // $ ./stream
   // "Log message ... 1"
   // "Log message ... 2"
   // "Log message ... 3"
}
