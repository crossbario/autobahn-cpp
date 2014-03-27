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

#ifndef AUTOBAHN_HPP
#define AUTOBAHN_HPP

#include <stdint.h>
#include <stdexcept>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <msgpack.hpp>

#include <boost/any.hpp>

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
//#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>
//#include <future>


namespace autobahn {

   typedef std::map<std::string, boost::any> anymap;

   typedef std::vector<boost::any> anyvec;

   // Calls
   //
   struct call_t {
      boost::promise<boost::any> m_res;
   };

   typedef std::map<uint64_t, call_t> calls_t;


   // Registrations
   //

   typedef boost::any (*endpoint_t) (const anyvec&, const anymap&);

   // WAMP registration ID -> endpoint
   typedef std::map<uint64_t, endpoint_t> endpoints_t;


   struct registration_t {
      uint64_t m_id;
   };

   struct register_request_t {
      register_request_t(endpoint_t endpoint = 0) : m_endpoint(endpoint) {};
      endpoint_t m_endpoint;
      boost::promise<registration_t> m_res;
   };

   typedef std::map<uint64_t, register_request_t> register_requests_t;


   typedef std::vector<msgpack::object> wamp_msg_t;


   class session {
      public:
         session(std::istream& in, std::ostream& out);

         void loop();

         void stop(int exit_code = 0);

         boost::future<int> join(const std::string& realm);

         void publish(const std::string& topic);

         void publish(const std::string& topic, const anyvec& args);

         void publish(const std::string& topic, const anymap& kwargs);

         void publish(const std::string& topic, const anyvec& args, const anymap& kwargs);

         void publish(const std::string& topic, const boost::any& arg1);

         void publish(const std::string& topic, const boost::any& arg1, const boost::any& arg2);

         void publish(const std::string& topic, const boost::any& arg1, const boost::any& arg2, const boost::any& arg3);

         boost::future<registration_t> provide(const std::string& procedure, endpoint_t endpoint);

         boost::any invoke(const std::string& procedure, anyvec& args);


         /**
          * Calls a remote procedure. Generic positional argument vector, generic return.
          */
         //boost::future<boost::any> call(const std::string& procedure);

         boost::future<boost::any> call(const std::string& procedure, const anyvec& args);

         boost::future<boost::any> call(const std::string& procedure, const anyvec& args, const anymap& kwargs);

         template <typename T>
         boost::future<T> call(const std::string& procedure, const anyvec& args, const anymap& kwargs) {
            return call(procedure, args, kwargs).then(boost::launch::deferred, [](boost::future<boost::any> f) {
               return boost::any_cast<T> (f.get());
            });
         }

#if 0
         /**
          * Calls a remote procedure. Typed positional arguments, generic return.
          */
         template <typename... Args>
         boost::future<boost::any> call(const std::string& procedure, const Args&... args) {
            std::cerr << "1" << std::endl;

            anyvec accumulated;
            return _call_1(procedure, accumulated, args...);
         }

         /**
          * Calls a remote procedure. Generic positional argument vector, typed return.
          */
         template <typename T>
         boost::future<T> call(const std::string& procedure, const anyvec& args) {
            std::cerr << "2" << std::endl;

            return call(procedure, args).then(boost::launch::deferred, [](boost::future<boost::any> f) {
               return boost::any_cast<T> (f.get());
            });
         }

         /**
          * Calls a remote procedure. Typed positional arguments, typed return.
          */
         template <typename T, typename... Args>
         boost::future<T> call_static(const std::string& procedure, const T& _default, const Args&... args) {
            std::cerr << "3" << std::endl;

            anyvec accumulated;
            return _call_2<T>(procedure, accumulated, args...);
         }


         /// Entry point into template recursion for typed argument accumulation, generic return.
         template <typename Arg, typename... Args>
         boost::future<boost::any> _call_1(const std::string& procedure, anyvec& accumulated, const Arg& arg, const Args&... args) {
            std::cerr << "4" << std::endl;

            accumulated.push_back(arg);
            return _call_1(procedure, accumulated, args...);
         }

         /// Terminal of template recursion for typed argument accumulation, generic return.
         template <typename Arg>
         boost::future<boost::any> _call_1(const std::string& procedure, anyvec& accumulated, const Arg& arg) {
            std::cerr << "5" << std::endl;

            accumulated.push_back(arg);
            return call(procedure, accumulated);
         }

         /// Entry point into template recursion for typed argument accumulation, typed return.
         template <typename T, typename Arg, typename... Args>
         boost::future<T> _call_2(const std::string& procedure, anyvec& accumulated, const Arg& arg, const Args&... args) {
            std::cerr << "6" << std::endl;

            accumulated.push_back(arg);
            return _call_2<T>(procedure, accumulated, args...);
         }

         /// Terminal of template recursion for typed argument accumulation, typed return.
         template <typename T, typename Arg>
         boost::future<T> _call_2(const std::string& procedure, anyvec& accumulated, const Arg& arg) {
            std::cerr << "7" << std::endl;

            accumulated.push_back(arg);
            return call<T>(procedure, accumulated);
         }
#endif

      private:

         void process_welcome(const wamp_msg_t& msg);

         void process_call_result(const wamp_msg_t& msg);

         void process_registered(const wamp_msg_t& msg);

         void process_invocation(const wamp_msg_t& msg);


         boost::any unpack_any(msgpack::object& obj);

         void unpack_anyvec(std::vector<msgpack::object>& raw_args, anyvec& args);


         void pack_any(const boost::any& value);

         void send();

         bool receive();

         void send_hello(const std::string& realm);

         bool m_stopped;

         std::istream& m_in;
         std::ostream& m_out;

         msgpack::sbuffer m_buffer;
         msgpack::packer<msgpack::sbuffer> m_packer;
         msgpack::unpacker m_unpacker;

         uint64_t m_session_id;
         uint64_t m_request_id;

         boost::promise<int> m_session_join;

         calls_t m_calls;

         register_requests_t m_register_requests;
         endpoints_t m_endpoints;
   };

   class ProtocolError : public std::runtime_error {
      public:
         ProtocolError(const std::string& msg);
   };


   class Unimplemented : public std::runtime_error {
      public:
         Unimplemented(const std::string& msg, int type_code = 0);
         virtual const char* what() const throw();
      private:
         const int m_type_code;
   };

}

//#include "../src/autobahn.cpp"

#endif // AUTOBAHN_HPP
