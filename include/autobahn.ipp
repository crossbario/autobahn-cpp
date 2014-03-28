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

//#include "autobahn.hpp"


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

   template<typename IStream, typename OStream>
   session<IStream, OStream>::session(IStream& in, OStream& out)
      : m_debug(true),
        m_stopped(false),
        m_in(in),
        m_out(out),
        m_packer(&m_buffer),
        m_session_id(0),
        m_request_id(0)
   {
//      receive_msg();
   }

/*
   void session<IStream, OStream>::stop(int exit_code) {
      std::cerr << "stopping .." << std::endl;
      m_stopped = true;
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
      exit(exit_code);
   }
*/

   template<typename IStream, typename OStream>
   void session<IStream, OStream>::start() {
      receive_msg();
   }


   template<typename IStream, typename OStream>
   boost::future<int> session<IStream, OStream>::join(const std::string& realm) {

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


   template<typename IStream, typename OStream>
   boost::future<registration> session<IStream, OStream>::provide(const std::string& procedure, endpoint_t endpoint) {
      return _provide(procedure, static_cast<endpoint_t> (endpoint));
   }


   template<typename IStream, typename OStream>
   boost::future<registration> session<IStream, OStream>::provide_v(const std::string& procedure, endpoint_v_t endpoint) {
      return _provide(procedure, static_cast<endpoint_v_t> (endpoint));
   }


   template<typename IStream, typename OStream>
   boost::future<registration> session<IStream, OStream>::providef_vm(const std::string& procedure, endpointf_vm_t endpoint) {
      return _provide(procedure, static_cast<endpointf_vm_t> (endpoint));
   }


   template<typename IStream, typename OStream>
   template<typename E>
   boost::future<registration> session<IStream, OStream>::_provide(const std::string& procedure, E endpoint) {

      // [REGISTER, Request|id, Options|dict, Procedure|uri]

      std::cerr << "OOOOOOOOOOO " << typeid(endpoint).name() << std::endl;
      std::cerr << "OOOOOOOOOOO " << typeid(E()).name() << std::endl;

      m_request_id += 1;
      m_register_requests[m_request_id] = register_request_t(endpoint);

      m_packer.pack_array(4);
      m_packer.pack(static_cast<int> (msg_code::REGISTER));
      m_packer.pack(m_request_id);
      m_packer.pack_map(0);
      m_packer.pack(procedure);
      send();

      return m_register_requests[m_request_id].m_res.get_future();
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::publish(const std::string& topic) {

      // [PUBLISH, Request|id, Options|dict, Topic|uri]

      m_request_id += 1;

      m_packer.pack_array(4);
      m_packer.pack(MSG_CODE_PUBLISH);
      m_packer.pack(m_request_id);
      m_packer.pack_map(0);
      m_packer.pack(topic);
      send();
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::publish(const std::string& topic, const anyvec& args) {

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


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::publish(const std::string& topic, const anymap& kwargs) {

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


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::publish(const std::string& topic, const anyvec& args, const anymap& kwargs) {

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


   template<typename IStream, typename OStream>
   boost::future<boost::any> session<IStream, OStream>::call(const std::string& procedure) {

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


   template<typename IStream, typename OStream>
   boost::future<boost::any> session<IStream, OStream>::call(const std::string& procedure, const anyvec& args) {

      if (m_debug) {
         std::cerr << "call()" << std::endl;
      }

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

/*
   boost::future<boost::any> session<IStream, OStream>::call(const std::string& procedure, const anymap& kwargs) {

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
*/

   template<typename IStream, typename OStream>
   boost::future<boost::any> session<IStream, OStream>::call(const std::string& procedure, const anyvec& args, const anymap& kwargs) {

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


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::pack_any(const boost::any& value) {

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

/*
   template<typename E>
   boost::future<registration> session<IStream, OStream>::provide(const std::string& procedure, E endpoint) {

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
*/
/*
   template
   boost::future<registration> session<IStream, OStream>::provide<endpoint_t>(const std::string& procedure, endpoint_t endpoint);
*/
/*
   template
   boost::future<registration> session<IStream, OStream>::provide<endpoint_v_t>(const std::string& procedure, endpoint_v_t endpoint);
*/

   template<typename IStream, typename OStream>
   void session<IStream, OStream>::process_welcome(const wamp_msg_t& msg) {
      m_session_id = msg[1].as<uint64_t>();
      m_session_join.set_value(m_session_id);
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::process_goodbye(const wamp_msg_t& msg) {
      if (!m_goodbye_sent) {

         // if we did not initiate closing, reply ..

         // [GOODBYE, Details|dict, Reason|uri]

         m_packer.pack_array(3);

         m_packer.pack(MSG_CODE_GOODBYE);
         m_packer.pack_map(0);
         m_packer.pack(std::string("wamp.error.goodbye_and_out"));
         send();

      } else {
         // we previously initiated closing, so this
         // is the peer reply
      }
      std::string reason = msg[2].as<std::string>();
      m_session_leave.set_value(reason);
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::unpack_anyvec(std::vector<msgpack::object>& raw_args, anyvec& args) {
      for (int i = 0; i < raw_args.size(); ++i) {
         args.push_back(unpack_any(raw_args[i]));
      }
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::unpack_anymap(std::map<std::string, msgpack::object>& raw_kwargs, anymap& kwargs) {
   }


   template<typename IStream, typename OStream>
   boost::any session<IStream, OStream>::unpack_any(msgpack::object& obj) {
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


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::process_invocation(const wamp_msg_t& msg) {

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
               std::map<std::string, msgpack::object> raw_kwargs;
               msg[5].convert(&raw_kwargs);
               unpack_anymap(raw_kwargs, kwargs);
            }
         }

         std::cerr << "==> 5" << std::endl;
         try {

            if ((endpoint->second).type() == typeid(endpoint_t)) {

               boost::any res = ( boost::any_cast<endpoint_t>(endpoint->second) )(args, kwargs);

               m_packer.pack_array(4);
               m_packer.pack(MSG_CODE_YIELD);
               m_packer.pack(request_id);
               m_packer.pack_map(0);
               m_packer.pack_array(1);
               pack_any(res);
               send();

            } else if ((endpoint->second).type() == typeid(endpoint_v_t)) {

               anyvec res = ( boost::any_cast<endpoint_v_t>(endpoint->second) )(args, kwargs);

               m_packer.pack_array(4);
               m_packer.pack(MSG_CODE_YIELD);
               m_packer.pack(request_id);
               m_packer.pack_map(0);
               pack_any(res);
               send();

            } else if ((endpoint->second).type() == typeid(endpointf_vm_t)) {

               boost::future<anyvecmap> f_res = ( boost::any_cast<endpointf_vm_t>(endpoint->second) )(args, kwargs);

               std::cerr << "++ 1" << std::endl;

               auto done = f_res.then([&](decltype(f_res) f) {

                  std::cerr << "++ 2 " << typeid(f).name() << std::endl;

                  anyvecmap res = f.get();

                  std::cerr << "++ 3" << std::endl;

                  m_packer.pack_array(5);
                  m_packer.pack(MSG_CODE_YIELD);
                  m_packer.pack(request_id);
                  m_packer.pack_map(0);
                  pack_any(res.first);
                  pack_any(res.second);
                  send();

                  std::cerr << "++ 4" << std::endl;
               });

               std::cerr << "++ 5" << std::endl;
               done.get();
               std::cerr << "++ 6" << std::endl;

            } else {
               // FIXME
               std::cerr << "FIX ME INVOCATION " << std::endl;
               std::cerr << typeid(endpoint_t).name() << std::endl;
               std::cerr << ((endpoint->second).type()).name() << std::endl;
            }
//            boost::any res = (endpoint->second)(args, kwargs);
//            boost::any res = (*(endpoint->second))(args, kwargs);

            // [YIELD, INVOCATION.Request|id, Options|dict]
            // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list]
            // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list, ArgumentsKw|dict]

         }
         catch (...) {
            std::cerr << "INVOCATION failed" << std::endl;
         }

         std::cerr << "==> 6" << std::endl;

      } else {
         throw ProtocolError("bogus INVOCATION message for non-registered registration ID");
      }
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::process_call_result(const wamp_msg_t& msg) {

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

      typename calls_t::iterator call = m_calls.find(request_id);

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


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::process_registered(const wamp_msg_t& msg) {

      // [REGISTERED, REGISTER.Request|id, Registration|id]

      if (msg.size() != 3) {
         throw ProtocolError("invalid REGISTERED message structure - length must be 3");
      }

      if (msg[1].type != msgpack::type::POSITIVE_INTEGER) {
         throw ProtocolError("invalid REGISTERED message structure - REGISTERED.Request must be an integer");
      }

      uint64_t request_id = msg[1].as<uint64_t>();

      typename register_requests_t::iterator register_request = m_register_requests.find(request_id);

      if (register_request != m_register_requests.end()) {

         if (msg[2].type != msgpack::type::POSITIVE_INTEGER) {
            throw ProtocolError("invalid REGISTERED message structure - REGISTERED.Registration must be an integer");
         }

         uint64_t registration_id = msg[2].as<uint64_t>();

         //std::cerr << "REGxx" << register_request->second.m_endpoint << std::endl;

         std::cerr << "XXX 2a " << (register_request->second).m_endpoint.type().name() << std::endl;

         m_endpoints[registration_id] = register_request->second.m_endpoint;

         std::cerr << "XXX 2b " << (m_endpoints[registration_id]).type().name() << std::endl;

         registration reg;
         reg.m_id = registration_id;

         register_request->second.m_res.set_value(reg);

      } else {
         throw ProtocolError("bogus REGISTERED message for non-pending request ID");
      }
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::receive_msg() {

      if (m_debug) {
         std::cerr << "RX preparing to receive message .." << std::endl;
      }

      // read 4 octets msg length prefix ..
      boost::asio::async_read(m_in,
         boost::asio::buffer(m_buffer_msg_len, sizeof(m_buffer_msg_len)),
         bind(&session<IStream, OStream>::got_msg_header, this, boost::asio::placeholders::error));
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::got_msg_header(const boost::system::error_code& error) {
      if (!error) {

         m_msg_len = ntohl(*((uint32_t*) &m_buffer_msg_len));

         if (m_debug) {
            std::cerr << "RX message (" << m_msg_len << " octets) ..." << std::endl;
         }

         // read actual message
         m_unpacker.reserve_buffer(m_msg_len);

         boost::asio::async_read(m_in,
            boost::asio::buffer(m_unpacker.buffer(), m_msg_len),
            bind(&session<IStream, OStream>::got_msg_body, this, boost::asio::placeholders::error));

      } else {
      }
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::got_msg_body(const boost::system::error_code& error) {
      if (!error) {

         if (m_debug) {
            std::cerr << "RX message received." << std::endl;
         }

         m_unpacker.buffer_consumed(m_msg_len);

         msgpack::unpacked result;

         while (m_unpacker.next(&result)) {

            msgpack::object obj(result.get());

            if (m_debug) {
               std::cout << "RX WAMP message: " << obj << std::endl;
            }

            got_msg(obj);
         }

         if (!m_stopped) {
            receive_msg();
         }

      } else {

      }
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::got_msg(const msgpack::object& obj) {

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
   }


   template<typename IStream, typename OStream>
   void session<IStream, OStream>::send() {

      if (m_debug) {
         std::cerr << "TX message (" << m_buffer.size() << " octets) ..." << std::endl;
      }

      // boost::asio::async_write(s, boost::asio::buffer(data, size), handler);
      // boost::asio::write(s, boost::asio::buffer(data, size));

      std::size_t written = 0;

      // write message length prefix
      uint32_t len = htonl(m_buffer.size());
      written += boost::asio::write(m_out, boost::asio::buffer((char*) &len, sizeof(len)));

      // write actual serialized message
      written += boost::asio::write(m_out, boost::asio::buffer(m_buffer.data(), m_buffer.size()));

      if (m_debug) {
         std::cerr << "TX message sent (" << written << " / " << (sizeof(len) + m_buffer.size()) << " octets)" << std::endl;
      }

      // clear serialization buffer
      m_buffer.clear();
   }
}
