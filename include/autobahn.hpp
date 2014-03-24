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

#include <msgpack.hpp>

#include <boost/any.hpp>

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>
//#include <future>


namespace autobahn {

   typedef std::map<std::string, boost::any> anymap;

   typedef std::vector<boost::any> anyvec;

   typedef boost::any (*callback) (autobahn::anyvec&);

   typedef std::map<std::string, callback> endpoints;

   struct call_t {
      boost::promise<boost::any> m_res;
   };

   typedef std::map<uint64_t, call_t> calls_t;


   class session {
      public:
         session(std::istream& in, std::ostream& out);

         void loop();

         boost::future<int> join(const std::string& realm);

         void publish(const std::string& topic);

         void publish(const std::string& topic, anyvec& args);

         void publish(const std::string& topic, anymap& kwargs);

         void publish(const std::string& topic, anyvec& args, anymap& kwargs);

         void publish(const std::string& topic, boost::any arg1);

         void publish(const std::string& topic, boost::any arg1, boost::any arg2);

         void publish(const std::string& topic, boost::any arg1, boost::any arg2, boost::any arg3);

         void registerproc(const std::string& procedure, callback endpoint);

         boost::any invoke(const std::string& procedure, anyvec& args);

         boost::future<boost::any> call(const std::string& procedure, anyvec& args);

      private:

         void pack_any(const boost::any& value);

         void send();

         bool receive();

         void send_hello(const std::string& realm);

         std::istream& m_in;
         std::ostream& m_out;

         msgpack::sbuffer m_buffer;
         msgpack::packer<msgpack::sbuffer> m_packer;
         msgpack::unpacker m_unpacker;

         uint64_t m_session_id;
         uint64_t m_request_id;

         boost::promise<int> m_session_join;

         endpoints m_endpoints;
         calls_t m_calls;
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

#endif // AUTOBAHN_HPP
