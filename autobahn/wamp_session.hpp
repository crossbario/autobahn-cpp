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

#ifndef AUTOBAHN_SESSION_HPP
#define AUTOBAHN_SESSION_HPP

#include "wamp_endpoints.hpp"
#include "wamp_message.hpp"

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/thread/future.hpp>
#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>
#include <map>
#include <msgpack.hpp>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if defined(_WIN32) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef ERROR
#undef ERROR
#endif

namespace autobahn {

class wamp_call;
class wamp_registration;
class wamp_register_request;
class wamp_subscription;
class wamp_subscribe_request;

/// Representation of a WAMP session.
template<typename IStream, typename OStream>
class session
{
public:

    /*!
     * Create a new WAMP session.
     *
     * \param in The input stream to run this session on.
     * \param out THe output stream to run this session on.
     */
    session(boost::asio::io_service& io, IStream& in, OStream& out, bool debug = false);

    /*!
     * Start listening on the IStream provided to the constructor
     * of this session.
     */
    inline
    boost::future<bool> start();

    /*!
     * Closes the IStream and the OStream provided to the constructor
     * of this session.
     */
    inline
    void stop();

    /*!
     * Join a realm with this session.
     *
     * \param realm The realm to join on the WAMP router connected to.
     * \return A future that resolves with the session ID when the realm was joined.
     */
    inline
    boost::future<uint64_t> join(const std::string& realm);

    /*!
     * Leave the realm.
     *
     * \param reason An optional WAMP URI providing a reason for leaving.
     * \return A future that resolves with the reason sent by the peer.
     */
    inline
    boost::future<std::string> leave(const std::string& reason = std::string("wamp.error.close_realm"));

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
     * Publish an event with both positional and keyword payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     * \param args The positional payload for the event.
     * \param kwargs The keyword payload for the event.
     */
    inline
    void publish(const std::string& topic, const anyvec& args, const anymap& kwargs);

    /*!
     * Subscribe a handler to a topic to receive events.
     *
     * \param topic The URI of the topic to subscribe to.
     * \param handler The handler that will receive events under the subscription.
     * \return A future that resolves to a autobahn::subscription
     */
    inline
    boost::future<wamp_subscription> subscribe(const std::string& topic, handler_t handler);

    /*!
     * Calls a remote procedure with no arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \return A future that resolves to the result of the remote procedure call.
     */
    inline
    boost::future<boost::any> call(const std::string& procedure);

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
     * \param options Options when registering a procedure.
     * \return A future that resolves to a autobahn::registration
     */
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_t endpoint, const provide_options& options = provide_options());
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_v_t endpoint, const provide_options& options = provide_options());
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_m_t endpoint, const provide_options& options = provide_options());
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_vm_t endpoint, const provide_options& options = provide_options());
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_f_t endpoint, const provide_options& options = provide_options());
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_fv_t endpoint, const provide_options& options = provide_options());
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_fm_t endpoint, const provide_options& options = provide_options());
    inline boost::future<wamp_registration> provide(const std::string& procedure, endpoint_fvm_t endpoint, const provide_options& options = provide_options());

private:

    template<typename E>
    inline boost::future<wamp_registration> _provide(const std::string& procedure, E endpoint, const provide_options& options);

    /// Process a WAMP ERROR message.
    inline void process_error(const wamp_message& message);

    /// Process a WAMP HELLO message.
    inline void process_welcome(const wamp_message& message);

    /// Process a WAMP RESULT message.
    inline void process_call_result(const wamp_message& message);

    /// Process a WAMP SUBSCRIBED message.
    inline void process_subscribed(const wamp_message& message);

    /// Process a WAMP EVENT message.
    inline void process_event(const wamp_message& message);

    /// Process a WAMP REGISTERED message.
    inline void process_registered(const wamp_message& message);

    /// Process a WAMP INVOCATION message.
    inline void process_invocation(const wamp_message& message);

    /// Process a WAMP GOODBYE message.
    inline void process_goodbye(const wamp_message& message);


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
    inline void receive_message();

    void got_handshake_reply(const boost::system::error_code& error);

    void got_message_header(const boost::system::error_code& error);

    void got_message_body(const boost::system::error_code& error);

    void got_message(const msgpack::object& obj);


    bool m_debug;

    bool m_stopped;

    boost::asio::io_service& m_io;

    /// Input stream this session runs on.
    IStream& m_in;

    /// Output stream this session runs on.
    OStream& m_out;


    char m_buffer_message_length[4];
    uint32_t m_message_length;

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

    /// Last request ID of outgoing WAMP requests.
    uint64_t m_request_id;


    bool m_goodbye_sent;

    boost::promise<std::string> m_session_leave;

    /// Synchronization for dealing with the rawsocket handshake
    boost::promise<bool> m_handshake;

    //////////////////////////////////////////////////////////////////////////////////////
    /// Caller

    /// Map of outstanding WAMP calls (request ID -> call).
    std::map<uint64_t, wamp_call> m_calls;


    //////////////////////////////////////////////////////////////////////////////////////
    /// Subscriber

    /// Map of outstanding WAMP subscribe requests (request ID -> subscribe request).
    std::map<uint64_t, wamp_subscribe_request> m_subscribe_requests;

    /// Map of subscribed handlers (subscription ID -> handler)
    std::multimap<uint64_t, handler_t> m_subscription_handlers;


    //////////////////////////////////////////////////////////////////////////////////////
    /// Callee

    /// Map of outstanding WAMP register requests (request ID -> register request).
    std::map<uint64_t, wamp_register_request> m_register_requests;

    /// Map of registered endpoints (registration ID -> endpoint)
    std::map<uint64_t, boost::any> m_endpoints;

    /// An unserialized, raw WAMP message.
    wamp_message m_message;
};

} // namespace autobahn

#include "wamp_session.ipp"

#endif // AUTOBAHN_SESSION_HPP
