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


int main () {
   WampSession session(std::cin, std::cout);
   session.send_hello("realm2");

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
