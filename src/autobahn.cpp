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

#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>


#include <cstdint>
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
      : m_stopped(false),
        m_in(in),
        m_out(out),
        m_packer(&m_buffer),
        m_session_id(0),
        m_request_id(0) {
   }

   void session::stop(int exit_code) {
      std::cerr << "stopping .." << std::endl;
      m_stopped = true;
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
      exit(exit_code);
   }


   boost::future<int> session::join(const std::string& realm) {

      // [HELLO, Realm|uri, Details|dict]

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

      return m_session_join.get_future();
   }


   void session::publish(const std::string& topic) {

      // [PUBLISH, Request|id, Options|dict, Topic|uri]

      m_request_id += 1;

      m_packer.pack_array(4);
      m_packer.pack(MSG_CODE_PUBLISH);
      m_packer.pack(m_request_id);
      m_packer.pack_map(0);
      m_packer.pack(topic);
      send();
   }


   void session::publish(const std::string& topic, const anyvec& args) {

      // [PUBLISH, Request|id, Options|dict, Topic|uri, Arguments|list]

      if (args.size() > 0) {

         m_request_id += 1;

         m_packer.pack_array(5);
         m_packer.pack(MSG_CODE_PUBLISH);
         m_packer.pack(m_request_id);
         m_packer.pack_map(0);
         m_packer.pack(topic);
         pack_any(args);
         send();
      } else {

         publish(topic);
      }
   }


   void session::publish(const std::string& topic, const anymap& kwargs) {

      // [PUBLISH, Request|id, Options|dict, Topic|uri, [], ArgumentsKw|dict]

      if (kwargs.size() > 0) {

         m_request_id += 1;

         m_packer.pack_array(6);
         m_packer.pack(MSG_CODE_PUBLISH);
         m_packer.pack(m_request_id);
         m_packer.pack_map(0);
         m_packer.pack(topic);
         m_packer.pack_array(0);
         pack_any(kwargs);
         send();
      } else {

         publish(topic);
      }
   }


   void session::publish(const std::string& topic, const anyvec& args, const anymap& kwargs) {

      // [PUBLISH, Request|id, Options|dict, Topic|uri, Arguments|list, ArgumentsKw|dict]

      if (kwargs.size() > 0) {

         m_request_id += 1;

         m_packer.pack_array(6);
         m_packer.pack(MSG_CODE_PUBLISH);
         m_packer.pack(m_request_id);
         m_packer.pack_map(0);
         m_packer.pack(topic);
         pack_any(args);
         pack_any(kwargs);
         send();
      } else {

         publish(topic, args);
      }
   }


   boost::future<boost::any> session::call(const std::string& procedure) {

      // [CALL, Request|id, Options|dict, Procedure|uri]

      m_request_id += 1;

      m_calls[m_request_id] = call_t();

      m_packer.pack_array(4);
      m_packer.pack(MSG_CODE_CALL);
      m_packer.pack(m_request_id);
      m_packer.pack_map(0);
      m_packer.pack(procedure);
      send();

      return m_calls[m_request_id].m_res.get_future();
   }


   boost::future<boost::any> session::call(const std::string& procedure, const anyvec& args) {

      // [CALL, Request|id, Options|dict, Procedure|uri, Arguments|list]

      if (args.size() > 0) {

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

      } else {
         return call(procedure);
      }
   }


   boost::future<boost::any> session::call(const std::string& procedure, const anymap& kwargs) {

      // [CALL, Request|id, Options|dict, Procedure|uri, [], ArgumentsKw|dict]

      if (kwargs.size() > 0) {

         m_request_id += 1;

         m_calls[m_request_id] = call_t();

         m_packer.pack_array(6);
         m_packer.pack(MSG_CODE_CALL);
         m_packer.pack(m_request_id);
         m_packer.pack_map(0);
         m_packer.pack(procedure);
         m_packer.pack_array(0);
         pack_any(kwargs);
         send();

         return m_calls[m_request_id].m_res.get_future();
      } else {
         return call(procedure);
      }
   }


   boost::future<boost::any> session::call(const std::string& procedure, const anyvec& args, const anymap& kwargs) {

      // [CALL, Request|id, Options|dict, Procedure|uri, Arguments|list, ArgumentsKw|dict]

      if (kwargs.size() > 0) {

         m_request_id += 1;

         m_calls[m_request_id] = call_t();

         m_packer.pack_array(6);
         m_packer.pack(MSG_CODE_CALL);
         m_packer.pack(m_request_id);
         m_packer.pack_map(0);
         m_packer.pack(procedure);
         pack_any(args);
         pack_any(kwargs);
         send();

         return m_calls[m_request_id].m_res.get_future();

      } else {
         return call(procedure, args);
      }
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

      } else if (value.type() == typeid(uint64_t)) {

         uint64_t val = boost::any_cast<uint64_t>(value);
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




   boost::future<registration> session::provide(const std::string& procedure, endpoint_v_t endpoint) {
   }

   boost::future<registration> session::provide(const std::string& procedure, endpoint_f_t endpoint) {
   }

   boost::future<registration> session::provide(const std::string& procedure, endpoint_t endpoint) {

      // [REGISTER, Request|id, Options|dict, Procedure|uri]

      m_request_id += 1;
      m_register_requests[m_request_id] = register_request_t(endpoint);

      m_packer.pack_array(4);
      m_packer.pack(MSG_CODE_REGISTER);
      m_packer.pack(m_request_id);
      m_packer.pack_map(0);
      m_packer.pack(procedure);
      send();

      return m_register_requests[m_request_id].m_res.get_future();
   }




/*
#include <unistd.h>

   std::cout.flush();
   close(STDOUT_FILENO);
   sleep(2);
   std::cerr << "EXIT" << std::endl;


*/

   void session::process_welcome(const wamp_msg_t& msg) {
      m_session_id = msg[1].as<uint64_t>();
      m_session_join.set_value(m_session_id);
   }


   void session::unpack_anyvec(std::vector<msgpack::object>& raw_args, anyvec& args) {
      for (int i = 0; i < raw_args.size(); ++i) {
         args.push_back(unpack_any(raw_args[i]));
      }
   }


   boost::any session::unpack_any(msgpack::object& obj) {
      switch (obj.type) {

         case msgpack::type::RAW:
            return boost::any(obj.as<std::string>());

         case msgpack::type::POSITIVE_INTEGER:
            return boost::any(obj.as<uint64_t>());

         case msgpack::type::NEGATIVE_INTEGER:
            return boost::any(obj.as<int64_t>());

         case msgpack::type::BOOLEAN:
            return boost::any(obj.as<bool>());

         case msgpack::type::DOUBLE:
            return boost::any(obj.as<double>());

         case msgpack::type::NIL:
            return boost::any();

         case msgpack::type::ARRAY:
            // FIXME
            {
               anyvec out_vec;
               std::vector<msgpack::object> in_vec;
               obj.convert(&in_vec);
               for (int i = 0; i < in_vec.size(); ++i) {
                  out_vec.push_back(unpack_any(in_vec[i]));
               }
               return out_vec;
               //std::cerr << "unprocess ARRAY" << std::endl;
            }

         case msgpack::type::MAP:
            // FIXME

         default:
            return boost::any();
      }
   }


   void session::process_invocation(const wamp_msg_t& msg) {

      // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict]
      // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list]
      // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list, CALL.ArgumentsKw|dict]

      std::cerr << "==> 1" << std::endl;

      if (msg.size() != 4 && msg.size() != 5 && msg.size() != 6) {
         throw ProtocolError("invalid INVOCATION message structure - length must be 4, 5 or 6");
      }

      if (msg[1].type != msgpack::type::POSITIVE_INTEGER) {
         throw ProtocolError("invalid INVOCATION message structure - INVOCATION.Request must be an integer");
      }
      uint64_t request_id = msg[1].as<uint64_t>();

      std::cerr << "==> 2" << std::endl;

      if (msg[2].type != msgpack::type::POSITIVE_INTEGER) {
         throw ProtocolError("invalid INVOCATION message structure - INVOCATION.Registration must be an integer");
      }
      uint64_t registration_id = msg[2].as<uint64_t>();

      endpoints_t::iterator endpoint = m_endpoints.find(registration_id);

      std::cerr << "==> 3" << std::endl;

      if (endpoint != m_endpoints.end()) {

         if (msg[3].type != msgpack::type::MAP) {
            throw ProtocolError("invalid INVOCATION message structure - Details must be a dictionary");
         }

         anyvec args;
         anymap kwargs;

         std::cerr << "==> 4" << std::endl;

         if (msg.size() > 4) {

            if (msg[4].type != msgpack::type::ARRAY) {
               throw ProtocolError("invalid INVOCATION message structure - INVOCATION.Arguments must be a list");
            }

            std::vector<msgpack::object> raw_args;
            msg[4].convert(&raw_args);
            unpack_anyvec(raw_args, args);

            if (msg.size() > 5) {
               // FIXME
            }
         }

         std::cerr << "==> 5" << std::endl;
         try {
            std::cerr << endpoint->second << std::endl;
            boost::any res = (*(endpoint->second))(args, kwargs);

            // [YIELD, INVOCATION.Request|id, Options|dict]
            // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list]
            // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list, ArgumentsKw|dict]

            m_packer.pack_array(4);
            m_packer.pack(MSG_CODE_YIELD);
            m_packer.pack(request_id);
            m_packer.pack_map(0);
            m_packer.pack_array(1);
            pack_any(res);
            send();
         }
         catch (...) {
            std::cerr << "INVOCATION failed" << std::endl;
         }

         std::cerr << "==> 6" << std::endl;

      } else {
         throw ProtocolError("bogus INVOCATION message for non-registered registration ID");
      }
   }


   void session::process_call_result(const wamp_msg_t& msg) {

      // [RESULT, CALL.Request|id, Details|dict]
      // [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list]
      // [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list, YIELD.ArgumentsKw|dict]

      if (msg.size() != 3 && msg.size() != 4 && msg.size() != 5) {
         throw ProtocolError("invalid RESULT message structure - length must be 3, 4 or 5");
      }

      if (msg[1].type != msgpack::type::POSITIVE_INTEGER) {
         throw ProtocolError("invalid RESULT message structure - CALL.Request must be an integer");
      }

      uint64_t request_id = msg[1].as<uint64_t>();

      calls_t::iterator call = m_calls.find(request_id);

      if (call != m_calls.end()) {

         if (msg[2].type != msgpack::type::MAP) {
            throw ProtocolError("invalid RESULT message structure - Details must be a dictionary");
         }

         if (msg.size() > 3) {

            if (msg[3].type != msgpack::type::ARRAY) {
               throw ProtocolError("invalid RESULT message structure - YIELD.Arguments must be a list");
            }

            std::vector<msgpack::object> raw_args;
            msg[3].convert(&raw_args);

            anyvec args;

            unpack_anyvec(raw_args, args);

            if (args.size() > 0) {
               call->second.m_res.set_value(args[0]);
            } else {
               call->second.m_res.set_value(boost::any());
            }

         } else {
            // empty result
            call->second.m_res.set_value(boost::any());
         }
      } else {
         throw ProtocolError("bogus RESULT message for non-pending request ID");
      }
   }


   void session::process_registered(const wamp_msg_t& msg) {

      // [REGISTERED, REGISTER.Request|id, Registration|id]

      if (msg.size() != 3) {
         throw ProtocolError("invalid REGISTERED message structure - length must be 3");
      }

      if (msg[1].type != msgpack::type::POSITIVE_INTEGER) {
         throw ProtocolError("invalid REGISTERED message structure - REGISTERED.Request must be an integer");
      }

      uint64_t request_id = msg[1].as<uint64_t>();

      register_requests_t::iterator register_request = m_register_requests.find(request_id);

      if (register_request != m_register_requests.end()) {

         if (msg[2].type != msgpack::type::POSITIVE_INTEGER) {
            throw ProtocolError("invalid REGISTERED message structure - REGISTERED.Registration must be an integer");
         }

         uint64_t registration_id = msg[2].as<uint64_t>();

         std::cerr << "REGxx" << register_request->second.m_endpoint << std::endl;

         m_endpoints[registration_id] = register_request->second.m_endpoint;

         registration reg;
         reg.m_id = registration_id;

         register_request->second.m_res.set_value(reg);

      } else {
         throw ProtocolError("bogus REGISTERED message for non-pending request ID");
      }
   }


   void session::loop() {
      int i = 0;
      try {

         while (!m_stopped && receive()) {

            msgpack::unpacked result;

            while (m_unpacker.next(&result)) {
               msgpack::object obj(result.get());

               std::cerr << "Received: " << obj << std::endl;

               if (obj.type != msgpack::type::ARRAY) {
                  throw ProtocolError("invalid message structure - message is not an array");
               }

               wamp_msg_t msg;
               obj.convert(&msg);

               if (msg.size() < 1) {
                  throw ProtocolError("invalid message structure - missing message code");
               }

               if (msg[0].type != msgpack::type::POSITIVE_INTEGER) {
                  throw ProtocolError("invalid message code type - not an integer");
               }

               int code = msg[0].as<int>();

               switch (code) {
                  case MSG_CODE_WELCOME:
                     process_welcome(msg);
                     break;

                  case MSG_CODE_RESULT:
                     process_call_result(msg);
                     break;

                  case MSG_CODE_REGISTERED:
                     process_registered(msg);
                     break;

                  case MSG_CODE_INVOCATION:
                     process_invocation(msg);
                     break;
               }

               //throw Unimplemented("WAMP message", 23);


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
      m_out.flush();
      m_buffer.clear();
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
