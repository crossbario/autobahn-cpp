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
#include <utility>
#include <vector>
#include <map>
#include <functional>


#include <msgpack.hpp>

#include <boost/any.hpp>

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>
//#include <future>


#include <boost/asio.hpp>



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


//   typedef boost::any (*endpoint_t) (const anyvec&, const anymap&);

   /// Endpoint type for use with session::provide(const std::string&, endpoint_t)
   typedef std::function<boost::any(const anyvec&, const anymap&)> endpoint_t;

   /// Endpoint type for use with session::provide(const std::string&, endpoint_v_t)
   typedef std::function<anyvec(const anyvec&, const anymap&)> endpoint_v_t;

   /// Endpoint type for use with session::provide(const std::string&, endpoint_m_t)
   typedef std::function<anymap(const anyvec&, const anymap&)> endpoint_m_t;

   /// Endpoint type for use with session::provide(const std::string&, endpoint_vm_t)
   typedef std::function<anyvecmap(const anyvec&, const anymap&)> endpoint_vm_t;


   /// Endpoint type for use with session::provide(const std::string&, endpointf_t)
   typedef std::function<boost::future<boost::any>(const anyvec&, const anymap&)> endpointf_t;

   /// Endpoint type for use with session::provide(const std::string&, endpointf_v_t)
   typedef std::function<boost::future<anyvec>(const anyvec&, const anymap&)> endpointf_v_t;

   /// Endpoint type for use with session::provide(const std::string&, endpointf_m_t)
   typedef std::function<boost::future<anymap>(const anyvec&, const anymap&)> endpointf_m_t;

   /// Endpoint type for use with session::provide(const std::string&, endpointf_vm_t)
   typedef std::function<boost::future<anyvecmap>(const anyvec&, const anymap&)> endpointf_vm_t;


   /// Represents a procedure registration.
   struct registration {
      uint64_t m_id;
   };


   /// Represents a topic subscription.
   struct subscription {
      uint64_t m_id;
   };


   /*!
    * A WAMP session.
    */
   template<typename IStream, typename OStream>
   class session {

      public:

         /*!
          * Create a new WAMP session.
          *
          * \param in The input stream to run this session on.
          * \param out THe output stream to run this session on.
          */
         session(boost::asio::io_service& io, IStream& in, OStream& out, bool debug = false);

         /*!
          * Join a realm with this session.
          *
          * \param realm The realm to join on the WAMP router connected to.
          * \return A future that resolves when the realm was joined.
          */
         inline
         boost::future<uint64_t> join(const std::string& realm);
         //boost::shared_future<int> join(const std::string& realm);

         inline
         boost::future<void> leave();

         /*!
          * Enter the session event loop. This will not return until the
          * session ends.
          */
         inline
         void start();

         /*!
          * Stop the whole program.
          */
         //void stop(int exit_code = 0);

         /*!
          * Publish an event with empty payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          */
         inline
         void publish(const std::string& topic);

         /*!
          * Publish an event with positional payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          * \param args The positional payload for the event.
          */
         inline
         void publish(const std::string& topic, const anyvec& args);

         /*!
          * Publish an event with keyword payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          * \param kwargs The keyword payload for the event.
          */
         inline
         void publish(const std::string& topic, const anymap& kwargs);

         /*!
          * Publish an event with both positional and keyword payload to a topic.
          *
          * \param topic The URI of the topic to publish to.
          * \param args The positional payload for the event.
          * \param kwargs The keyword payload for the event.
          */
         inline
         void publish(const std::string& topic, const anyvec& args, const anymap& kwargs);

         /*!
          * Calls a remote procedure with no arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         inline
         boost::future<boost::any> call(const std::string& procedure);

         inline void foo() {
            std::cerr << "FOOO" << std::endl;
         }

         /*!
          * Calls a remote procedure with positional arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \param args The positional arguments for the call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         inline
         boost::future<boost::any> call(const std::string& procedure, const anyvec& args);

         /*!
          * Calls a remote procedure with keyword arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \param kwargs The keyword arguments for the call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         //boost::future<boost::any> call(const std::string& procedure, const anymap& kwargs);

         /*!
          * Calls a remote procedure with positional and keyword arguments.
          *
          * \param procedure The URI of the remote procedure to call.
          * \param args The positional arguments for the call.
          * \param kwargs The keyword arguments for the call.
          * \return A future that resolves to the result of the remote procedure call.
          */
         inline
         boost::future<boost::any> call(const std::string& procedure, const anyvec& args, const anymap& kwargs);

         /*!
          * Register an endpoint as a procedure that can be called remotely.
          *
          * \param procedure The URI under which the procedure is to be exposed.
          * \param endpoint The endpoint to be exposed as a remotely callable procedure.
          * \return A future that resolves to a autobahn::registration
          */
         inline boost::future<registration> provide(const std::string& procedure, endpoint_t endpoint);

         inline boost::future<registration> provide_v(const std::string& procedure, endpoint_v_t endpoint);

         inline boost::future<registration> providef_vm(const std::string& procedure, endpointf_vm_t endpoint);

      private:

         template<typename E>
         inline boost::future<registration> _provide(const std::string& procedure, E endpoint);


         /// An outstanding WAMP call.
         struct call_t {
            boost::promise<boost::any> m_res;
         };

         /// Map of outstanding WAMP calls (request ID -> call).
         typedef std::map<uint64_t, call_t> calls_t;


         /// An outstanding WAMP register request.
         struct register_request_t {
            register_request_t() {};
            register_request_t(boost::any endpoint) : m_endpoint(endpoint) {};
            boost::any m_endpoint;
            boost::promise<registration> m_res;
         };

         /// Map of outstanding WAMP register requests (request ID -> register request).
         typedef std::map<uint64_t, register_request_t> register_requests_t;

         /// Map of registered endpoints (registration ID -> endpoint)
         typedef std::map<uint64_t, boost::any> endpoints_t;

         /// An unserialized, raw WAMP message.
         typedef std::vector<msgpack::object> wamp_msg_t;


         /// Process a WAMP HELLO message.
         inline void process_welcome(const wamp_msg_t& msg);

         /// Process a WAMP RESULT message.
         inline void process_call_result(const wamp_msg_t& msg);

         /// Process a WAMP REGISTERED message.
         inline void process_registered(const wamp_msg_t& msg);

         /// Process a WAMP INVOCATION message.
         inline void process_invocation(const wamp_msg_t& msg);

         /// Process a WAMP GOODBYE message.
         inline void process_goodbye(const wamp_msg_t& msg);


         /// Unpacks any MsgPack object into boost::any value.
         inline boost::any unpack_any(msgpack::object& obj);

         /// Unpacks MsgPack array into anyvec.
         inline void unpack_anyvec(std::vector<msgpack::object>& raw_args, anyvec& args);

         /// Unpacks MsgPack map into anymap.
         inline void unpack_anymap(std::map<std::string, msgpack::object>& raw_kwargs, anymap& kwargs);

         /// Pack any value into serializion buffer.
         inline void pack_any(const boost::any& value);

         /// Send out message serialized in serialization buffer to ostream.
         inline void send();

         /// Receive one message from istream in m_unpacker.
         inline void receive_msg();


         void got_msg_header(const boost::system::error_code& error);

         void got_msg_body(const boost::system::error_code& error);

         void got_msg(const msgpack::object& obj);


         bool m_debug;

         bool m_stopped;

         boost::asio::io_service& m_io;

         /// Input stream this session runs on.
         IStream& m_in;

         /// Output stream this session runs on.
         OStream& m_out;


         char m_buffer_msg_len[4];
         uint32_t m_msg_len;

         boost::promise<boost::any> m_test_promise;


         /// MsgPack serialization buffer.
         msgpack::sbuffer m_buffer;

         /// MsgPacker serialization packer.
         msgpack::packer<msgpack::sbuffer> m_packer;

         /// MsgPack unserialization unpacker.
         msgpack::unpacker m_unpacker;

         /// WAMP session ID (if the session is joined to a realm).
         uint64_t m_session_id;

         /// Future to be fired when session was joined.
         boost::promise<uint64_t> m_session_join;

         bool m_goodbye_sent;

         boost::promise<std::string> m_session_leave;

         /// Last request ID of outgoing WAMP requests.
         uint64_t m_request_id;

         /// Map of WAMP call ID -> call
         calls_t m_calls;

         /// Map of WAMP register request ID -> register request
         register_requests_t m_register_requests;

         /// Map of WAMP registration ID -> endpoint
         endpoints_t m_endpoints;

         /// WAMP message type codes.
         enum class msg_code : int {
            HELLO = 1,
            WELCOME = 2,
            ABORT = 3,
            CHALLENGE = 4,
            AUTHENTICATE = 5,
            GOODBYE = 6,
            HEARTBEAT = 7,
            ERROR = 8,
            PUBLISH = 16,
            PUBLISHED = 17,
            SUBSCRIBE = 32,
            SUBSCRIBED = 33,
            UNSUBSCRIBE = 34,
            UNSUBSCRIBED = 35,
            EVENT = 36,
            CALL = 48,
            CANCEL = 49,
            RESULT = 50,
            REGISTER = 64,
            REGISTERED = 65,
            UNREGISTER = 66,
            UNREGISTERED = 67,
            INVOCATION = 68,
            INTERRUPT = 69,
            YIELD = 70
         };
   };


   class ProtocolError : public std::runtime_error {
      public:
         ProtocolError(const std::string& msg) : std::runtime_error(msg) {};
   };

}

#include "autobahn.ipp"

#endif // AUTOBAHN_HPP
