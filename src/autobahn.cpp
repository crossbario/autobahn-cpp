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
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "autobahn.hpp"


#define MSG_CODE_HELLO 1
#define MSG_CODE_WELCOME 2
#define MSG_CODE_ABORT 3
#define MSG_CODE_CHALLENGE 4
#define MSG_CODE_AUTHENTICATE 5
#define MSG_CODE_GOODBYE 6
#define MSG_CODE_HEARTBEAT 7
#define MSG_CODE_ERROR 8
#define MSG_CODE_PUBLISH 16
#define MSG_CODE_PUBLISHED 17
#define MSG_CODE_SUBSCRIBE 32
#define MSG_CODE_SUBSCRIBED 33
#define MSG_CODE_UNSUBSCRIBE 34
#define MSG_CODE_UNSUBSCRIBED 35
#define MSG_CODE_EVENT 36
#define MSG_CODE_CALL 48
#define MSG_CODE_CANCEL 49
#define MSG_CODE_RESULT 50
#define MSG_CODE_REGISTER 64
#define MSG_CODE_REGISTERED 65
#define MSG_CODE_UNREGISTER 66
#define MSG_CODE_UNREGISTERED 67
#define MSG_CODE_INVOCATION 68
#define MSG_CODE_INTERRUPT 69
#define MSG_CODE_YIELD 70


namespace autobahn {

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



   session::session(std::istream& in, std::ostream& out)
      : m_in(in),
        m_out(out),
        m_packer(&m_buffer),
        m_session_id(0),
        m_request_id(0) {
   }


   boost::future<int> session::join(const std::string& realm) {
      //boost::promise<int> p;
      //p.set_value(23);
      //m_session_join = new boost::promise<int>();
      send_hello(realm);
      //m_session_join.set_value(23);
      return m_session_join.get_future();
   }


   void session::registerproc(const std::string& procedure, callback endpoint) {
      std::cerr << "registering procedure: " << procedure << std::endl;
      m_endpoints[procedure] = endpoint;
   }


   boost::any session::invoke(const std::string& procedure, anyvec& args) {
      endpoints::iterator ep = m_endpoints.find(procedure);
      if (ep == m_endpoints.end()) {
         std::cerr << "procedure not found" << std::endl;
      } else {
         std::cerr << "invoking: " << ep->first << std::endl;
         return (*(ep->second)) (args);
      }
      return boost::any();
   }

   boost::future<boost::any> session::call(const std::string& procedure, anyvec& args) {
      m_request_id += 1;

      m_calls[m_request_id] = call_t();

      m_packer.pack_array(5);
      m_packer.pack(MSG_CODE_CALL);
      m_packer.pack(m_request_id);
      m_packer.pack_map(0);
      m_packer.pack(procedure);
      pack_any(args);
      send();

      return m_calls[m_request_id].m_res.get_future();
   }


   void session::pack_any(const boost::any& value) {

      if (value.empty()) {

         m_packer.pack_nil();

      } else if (value.type() == typeid(anyvec)) {

         anyvec v = boost::any_cast<anyvec>(value);

         m_packer.pack_array(v.size());

         anyvec::iterator it = v.begin();
         while (it != v.end()) {
            pack_any(*it);
            ++it;
         }

      } else if (value.type() == typeid(anymap)) {

         anymap m = boost::any_cast<anymap>(value);

         m_packer.pack_map(m.size());

         anymap::iterator it = m.begin();
         while (it != m.end()) {
            m_packer.pack(it->first); // std::string
            pack_any(it->second);
            ++it;
         }

      } else if (value.type() == typeid(int)) {

         int val = boost::any_cast<int>(value);
         m_packer.pack(val);

      } else if (value.type() == typeid(bool)) {

         bool val = boost::any_cast<bool>(value);
         m_packer.pack(val);

      } else if (value.type() == typeid(float)) {

         float val = boost::any_cast<float>(value);
         m_packer.pack(val);

      } else if (value.type() == typeid(double)) {

         double val = boost::any_cast<double>(value);
         m_packer.pack(val);

      } else if (value.type() == typeid(std::string)) {

         std::string val = boost::any_cast<std::string>(value);
         m_packer.pack(val);

      } else {
         //std::cerr << "? ";
      }
   }


   void session::publish(const std::string& topic) {

      m_packer.pack_array(4);
      m_packer.pack(MSG_CODE_PUBLISH);
      m_packer.pack(1);
      m_packer.pack_map(0);
      m_packer.pack(topic);
      send();
   }


   void session::publish(const std::string& topic, anyvec& args) {

      if (args.size() > 0) {
         m_packer.pack_array(5);
         m_packer.pack(MSG_CODE_PUBLISH);
         m_packer.pack(1);
         m_packer.pack_map(0);
         m_packer.pack(topic);
         pack_any(args);
         send();
      } else {
         publish(topic);
      }
   }


   void session::publish(const std::string& topic, anyvec& args, anymap& kwargs) {

      if (kwargs.size() > 0) {
         m_packer.pack_array(6);
         m_packer.pack(MSG_CODE_PUBLISH);
         m_packer.pack(1);
         m_packer.pack_map(0);
         m_packer.pack(topic);
         pack_any(args);
         pack_any(kwargs);
         send();
      } else {
         publish(topic, args);
      }
   }


   void session::publish(const std::string& topic, anymap& kwargs) {

      if (kwargs.size() > 0) {
         m_packer.pack_array(6);
         m_packer.pack(MSG_CODE_PUBLISH);
         m_packer.pack(1);
         m_packer.pack_map(0);
         m_packer.pack(topic);
         m_packer.pack_array(0);
         pack_any(kwargs);
         send();
      } else {
         publish(topic);
      }
   }


   void session::publish(const std::string& topic, boost::any arg1) {
      anyvec v;
      v.push_back(arg1);
      publish(topic, v);
   }


   void session::publish(const std::string& topic, boost::any arg1, boost::any arg2) {
      anyvec v;
      v.push_back(arg1);
      v.push_back(arg2);
      publish(topic, v);
   }


   void session::publish(const std::string& topic, boost::any arg1, boost::any arg2, boost::any arg3) {
      anyvec v;
      v.push_back(arg1);
      v.push_back(arg2);
      v.push_back(arg3);
      publish(topic, v);
   }


   void session::loop() {
      int i = 0;
      try {

         while (receive()) {

            std::cerr << "Got mesg: " << std::endl;
            std::cerr.flush();

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

               if (code == MSG_CODE_WELCOME) {

                  std::cerr << "firing welcome" << std::endl;
                  std::cerr.flush();

                  m_session_join.set_value(23);

                  std::cerr << "welcome fired" << std::endl;
                  std::cerr.flush();
               }

               //throw Unimplemented("WAMP message", 23);


               std::cerr << (obj.type == msgpack::type::ARRAY) << " : " << obj << std::endl;
               std::cerr.flush();
/*
                std::cerr << msg.size() << std::endl;
                if (msg[0] == MSG_CODE_HELLO) {
                  std::cerr << "HELLO" << std::endl;
                  std::map<std::string, msgpack::object> details;
                  std::map<std::string, msgpack::object>::iterator details_it;

                  msg[2].convert(&details);
                  std::cerr << details.size() << std::endl;

                  details_it = details.find("roles");
                  std::cerr << details_it->second << std::endl;
                }
*/
            }
            ++i;
         }
         std::cerr << "processed " << i << std::endl;
      }
      catch (ProtocolError& e) {
         std::cerr << "ProtocolError: " << e.what() << std::endl;
         std::cerr.flush();
      }
      catch (Unimplemented& e) {
         std::cerr << "Not implemented: " << e.what() << std::endl;
         std::cerr.flush();
      }
      catch (...) {
         std::cerr << "Unknown problem" << std::endl;
         std::cerr.flush();
      }
   }


   bool session::receive() {

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


   void session::send() {
      uint32_t len = htonl(m_buffer.size());
      m_out.write((char*) &len, 4);
      m_out.write(m_buffer.data(), m_buffer.size());
      //m_out.flush();
      m_buffer.clear();
      //std::cerr << "SENT" << std::endl;
   }


   void session::send_hello(const std::string& realm) {

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

}
