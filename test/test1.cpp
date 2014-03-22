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
#include <vector>
#include <msgpack.hpp>

#include "autobahn.hpp"

#if 0
int main() {
   std::cout << "hello" << add2(2, 3) << std::endl;
}
#else


#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>

namespace myprotocol {
   using namespace msgpack::type;
   using msgpack::define;

   struct Get : define< tuple<uint32_t, std::string> > {
      Get() { }

      Get(uint32_t f, const std::string& k) :
         define_type(msgpack_type(f, k)) { }

      uint32_t& flags() { return get<0>(); }
      std::string& key() { return get<1>(); }
   };

   struct Put : define< tuple<uint32_t, std::string, raw_ref> > {
      Put() { }
      Put(uint32_t f, const std::string& k, const char* valref, uint32_t vallen) :
         define_type(msgpack_type( f, k, raw_ref(valref,vallen) )) { }
      uint32_t& flags() { return get<0>(); }
      std::string& key() { return get<1>(); }
      raw_ref& value() { return get<2>(); }
   };

   struct MultiGet : define< std::vector<Get> > {
   };
}


int main(void) {
   // This is target object.
   std::vector<std::string> target;
   target.push_back("Hello,");
   target.push_back("World!");

   // Serialize it.
   msgpack::sbuffer sbuf;  // simple buffer
   msgpack::pack(&sbuf, target);

   // Deserialize the serialized data.
   msgpack::unpacked msg;    // includes memory pool and deserialized object
   msgpack::unpack(&msg, sbuf.data(), sbuf.size());
   msgpack::object obj = msg.get();

   // Print the deserialized object to stdout.
   std::cout << obj << std::endl;    // ["Hello," "World!"]

   // Convert the deserialized object to staticaly typed object.
   std::vector<std::string> result;
   obj.convert(&result);

   // If the type is mismatched, it throws msgpack::type_error.
   //obj.as<int>();  // type is mismatched, msgpack::type_error is thrown
}

#endif
