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

#include <cstdint>
#include <stdexcept>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <utility>


#include <msgpack.hpp>

#include <boost/any.hpp>

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
//#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>
//#include <future>


/*!
 * Autobahn namespace.
 */
namespace autobahn {

   /// A map holding any values and string keys.
   typedef std::map<std::string, boost::any> anymap;

   /// A vector holding any values.
   typedef std::vector<boost::any> anyvec;

   /// A pair of ::anyvec and ::anymap.
   typedef std::pair<anyvec, anymap> anyvecmap;


   /// Endpoint type for use with session::provide(const std::string&, endpoint_t)
   typedef boost::any (*endpoint_t) (const anyvec&, const anymap&);

   typedef boost::future<boost::any> (*endpoint_f_t) (const anyvec&, const anymap&);

   typedef anyvec (*endpoint_v_t) (const anyvec&, const anymap&);

   /// Represents a procedure registration.
   struct registration {
      uint64_t m_id;
   };



   /*!
    * A WAMP session.
    */
   class session {

      public:

         /*!
          * Create a new WAMP session.
          *
          * \param in The input stream to run this session on.
          * \param out THe output stream to run this session on.
          */
         session(std::istream& in, std::ostream& out);

         /*!
          * Join a realm with this session.
          *
          * \param realm The realm to join on the WAMP router connected to.
          * \return A future that resolves when the realm was joined.
          */
         boost::future<int> join(const std::string& realm);

         /*!
          * Enter the session event loop. This will not return until the
          * session ends.
          */
         void loop();

         /*!
          * Stop the whole program.
          */
         void stop(int exit_code = 0);

         /*!
          * Publish an event with empty payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          */
         void publish(const std::string& topic);

         /*!
          * Publish an event with positional payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          * \param args The positional payload for the event.
          */
         void publish(const std::string& topic, const anyvec& args);

         /*!
          * Publish an event with keyword payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          * \param kwargs The keyword payload for the event.
          */
         void publish(const std::string& topic, const anymap& kwargs);

         /*!
          * Publish an event with both positional and keyword payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          * \param args The positional payload for the event.
          * \param kwargs The keyword payload for the event.
          */
         void publish(const std::string& topic, const anyvec& args, const anymap& kwargs);

         /*!
          * Calls a remote procedure with no arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         boost::future<boost::any> call(const std::string& procedure);

         /*!
          * Calls a remote procedure with positional arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \param args The positional arguments for the call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         boost::future<boost::any> call(const std::string& procedure, const anyvec& args);

         /*!
          * Calls a remote procedure with keyword arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \param kwargs The keyword arguments for the call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         boost::future<boost::any> call(const std::string& procedure, const anymap& kwargs);

         /*!
          * Calls a remote procedure with positional and keyword arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \param args The positional arguments for the call.
          * \param kwargs The keyword arguments for the call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         boost::future<boost::any> call(const std::string& procedure, const anyvec& args, const anymap& kwargs);



         /*!
          * Register an endpoint as a procedure that can be called remotely.
          *
          * \param procedure The URI under which the procedure is to be exposed.
          * \param endpoint The endpoint to be exposed as a remotely callable procedure.
          * \return A future that resolves to a autobahn::registration
          */
         boost::future<registration> provide(const std::string& procedure, endpoint_t endpoint);

         /*!
          *
          */
         boost::future<registration> provide(const std::string& procedure, endpoint_f_t endpoint);

         /*!
          *
          */
         boost::future<registration> provide(const std::string& procedure, endpoint_v_t endpoint);


#if 0
         /*!
          *
          */
         template <typename T>
         boost::future<T> call(const std::string& procedure, const anyvec& args, const anymap& kwargs) {
            return call(procedure, args, kwargs).then(boost::launch::deferred, [](boost::future<boost::any> f) {
               return boost::any_cast<T> (f.get());
            });
         }

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

         // Calls
         //
         struct call_t {
            boost::promise<boost::any> m_res;
         };

         typedef std::map<uint64_t, call_t> calls_t;


         // Registrations
         //



         // WAMP registration ID -> endpoint
         typedef std::map<uint64_t, endpoint_t> endpoints_t;


         struct register_request_t {
            register_request_t(endpoint_t endpoint = 0) : m_endpoint(endpoint) {};
            endpoint_t m_endpoint;
            boost::promise<registration> m_res;
         };

         typedef std::map<uint64_t, register_request_t> register_requests_t;


         typedef std::vector<msgpack::object> wamp_msg_t;


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
