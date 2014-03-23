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

#include <stdint.h>
#include "autobahn.hpp"


#include <arpa/inet.h>


#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <sstream>


ProtocolError::ProtocolError(const std::string& msg)
 : std::runtime_error(msg) {
}


Unimplemented::Unimplemented(const std::string& msg, int type_code)
   : std::runtime_error(msg), m_type_code(type_code) {
};

const char* Unimplemented::what() const throw() {
   if (m_type_code) {
      std::ostringstream s;
      s << std::runtime_error::what() << " (type code " << m_type_code << ")";
      return s.str().c_str();
   } else {
      return std::runtime_error::what();
   }
}



WampSession::WampSession(std::istream& in, std::ostream& out)
   : m_in(in),
     m_out(out),
     m_packer(&m_buffer),
     m_session_id(0) {
}

void WampSession::publish(const std::string& topic, args& args) {

   int request_id = 999;

   if (args.size() > 0) {
      m_packer.pack_array(5);
   } else {
      m_packer.pack_array(4);      
   }

   m_packer.pack(MSG_CODE_PUBLISH);
   m_packer.pack(request_id);
   m_packer.pack_map(0);
   m_packer.pack(topic);
   if (args.size() > 0) {
      m_packer.pack_array(args.size());
      for (int i = 0; i < args.size(); ++i) {
         m_packer.pack(args[i]);
      }
   }

   send();
}


void WampSession::publish(const std::string& topic, Value& args) {

   int request_id = 999;

   if (args._args.size() > 0) {
      m_packer.pack_array(5);
   } else {
      m_packer.pack_array(4);      
   }

   m_packer.pack(MSG_CODE_PUBLISH);
   m_packer.pack(request_id);
   m_packer.pack_map(0);
   m_packer.pack(topic);
   if (args._args.size() > 0) {
      m_packer.pack_array(args._args.size());
      for (int i = 0; i < args._args.size(); ++i) {
         m_packer.pack(args._args[i]);
      }
   }

   send();
}


using boost::any_cast;

void WampSession::pack_any(const boost::any& value) {

   if (value.type() == typeid(anyvec)) {

      anyvec v = any_cast<anyvec>(value);

      m_packer.pack_array(v.size());

      anyvec::iterator it = v.begin();
      while (it != v.end()) {
         pack_any(*it);
         ++it;
      }

   } else if (value.type() == typeid(anymap)) {

      anymap m = any_cast<anymap>(value);

      m_packer.pack_map(m.size());

      anymap::iterator it = m.begin();
      while (it != m.end()) {
         m_packer.pack(it->first); // std::string
         pack_any(it->second);
         ++it;
      }

   } else if (value.type() == typeid(int)) {

      int val = any_cast<int>(value);
      m_packer.pack(val);

   } else if (value.type() == typeid(bool)) {

      bool val = any_cast<bool>(value);
      m_packer.pack(val);

   } else if (value.type() == typeid(float)) {

      float val = any_cast<float>(value);
      m_packer.pack(val);

   } else if (value.type() == typeid(double)) {

      double val = any_cast<double>(value);
      m_packer.pack(val);

   } else if (value.type() == typeid(std::string)) {

      std::string val = any_cast<std::string>(value);
      m_packer.pack(val);

   } else {
      //std::cout << "? ";
   }
}

void WampSession::publish(const std::string& topic, anyvec& args) {

   m_packer.pack_array(5);
   m_packer.pack(MSG_CODE_PUBLISH);
   m_packer.pack(1);
   m_packer.pack_map(0);
   m_packer.pack(topic);
   pack_any(args);
   send();
}


void WampSession::process() {
   int i = 0;
   try {

      while (receive()) {

         msgpack::unpacked result;

         //m_unpacker.next(&result)


         while (m_unpacker.next(&result)) {
            msgpack::object obj(result.get());

            if (obj.type != msgpack::type::ARRAY) {
               throw ProtocolError("invalid message structure - message is not an array");
            }

            std::vector<msgpack::object> msg;
            obj.convert(&msg);

            if (msg.size() < 1) {
               throw ProtocolError("invalid message structure - missing message code");
            }

            if (msg[0].type != msgpack::type::POSITIVE_INTEGER) {
               throw ProtocolError("invalid message code type - not an integer");
            }

            int code = msg[0].as<int>();

            //throw Unimplemented("WAMP message", 23);


            std::cout << (obj.type == msgpack::type::ARRAY) << " : " << obj << std::endl;

             std::cout << msg.size() << std::endl;
             if (msg[0] == MSG_CODE_HELLO) {
               std::cout << "HELLO" << std::endl;
               std::map<std::string, msgpack::object> details;
               std::map<std::string, msgpack::object>::iterator details_it;

               msg[2].convert(&details);
               std::cout << details.size() << std::endl;

               details_it = details.find("roles");
               std::cout << details_it->second << std::endl;
             }

         }
         ++i;
      }
      std::cout << "processed " << i << std::endl;
   }
   catch (ProtocolError& e) {
      std::cerr << "ProtocolError: " << e.what() << std::endl;
   }
   catch (Unimplemented& e) {
      std::cerr << "Not implemented: " << e.what() << std::endl;
   }
   catch (...) {
      std::cerr << "Unknown problem" << std::endl;
   }
}



bool WampSession::receive() {
/*
0
1
0
1
0

   std::cout << m_in.gcount() << std::endl;
   std::cout << m_in.eof() << std::endl;
   std::cout << m_in.good() << std::endl;
   std::cout << m_in.fail() << std::endl;
   std::cout << m_in.bad() << std::endl;
*/

   char blen[4];
   m_in.read(blen, 4);
   if (m_in.eof() || m_in.fail()) {
      return false;
   }


   uint32_t len = ntohl(*((uint32_t*) &blen));
   m_unpacker.reserve_buffer(len);
   m_in.read(m_unpacker.buffer(), len);
   if (m_in.eof() || m_in.fail()) {
      return false;
   }

   m_unpacker.buffer_consumed(len);

   return true;
}


void WampSession::send() {
   uint32_t len = htonl(m_buffer.size());
   m_out.write((char*) &len, 4);
   m_out.write(m_buffer.data(), m_buffer.size());
   m_buffer.clear();
}


void WampSession::send_hello(const std::string& realm) {

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

   send();
}
