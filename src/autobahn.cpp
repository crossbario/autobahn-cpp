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

#include "autobahn.hpp"

int add2(int a, int b) {
   return a + b;
}


WampSession::WampSession(std::istream& in, std::ostream& out)
   : m_in(in), m_out(out), m_packer(&m_buffer) {
}


void WampSession::send_hello(const std::string& realm) {
   m_buffer.clear();

   m_packer.pack_array(3);

   m_packer.pack(MSG_CODE_HELLO);
   m_packer.pack(realm);

   m_packer.pack_map(1);
   m_packer.pack(std::string("roles"));

   m_packer.pack_map(4);

   m_packer.pack(std::string("caller"));
   m_packer.pack_map(0);

   m_packer.pack(std::string("callee"));
   m_packer.pack_map(0);

   m_packer.pack(std::string("publisher"));
   m_packer.pack_map(0);

   m_packer.pack(std::string("subscriber"));
   m_packer.pack_map(0);

   m_out.write(m_buffer.data(), m_buffer.size());
}
