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

#include "wamp_call_result.hpp"
#include "wamp_event_handler.hpp"
#include "wamp_message.hpp"
#include "wamp_procedure.hpp"

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/asio.hpp>
#include <boost/thread/future.hpp>
#include <cstdint>
#include <functional>
#include <istream>
#include <ostream>
#include <map>
#include <memory>
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
class wamp_register_request;
class wamp_registration;
class wamp_subscribe_request;
class wamp_subscription;
class wamp_unsubscribe_request;

/// Representation of a WAMP session.
template<typename IStream, typename OStream>
class wamp_session : public std::enable_shared_from_this<wamp_session<IStream, OStream>>
{
public:

    /*!
     * Create a new WAMP session.
     *
     * \param in The input stream to run this session on.
     * \param out THe output stream to run this session on.
     */
    wamp_session(boost::asio::io_service& io, IStream& in, OStream& out, bool debug = false);

    /*!
     * Start listening on the IStream provided to the constructor
     * of this session.
     *
     * \return A future that resolves to true if the session was successfully
     *         started and false otherwise.
     */
    boost::future<bool> start();

    /*!
     * Closes the IStream and the OStream provided to the constructor
     * of this session.
     */
    void stop();

    /*!
     * Join a realm with this session.
     *
     * \param realm The realm to join on the WAMP router connected to.
     * \return A future that resolves with the session ID when the realm was joined.
     */
    boost::future<uint64_t> join(const std::string& realm);

    /*!
     * Leave the realm.
     *
     * \param reason An optional WAMP URI providing a reason for leaving.
     * \return A future that resolves with the reason sent by the peer.
     */
    boost::future<std::string> leave(const std::string& reason = std::string("wamp.error.close_realm"));

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
     * \param arguments The positional payload for the event.
     */
    template <typename ARGUMENTS>
    void publish(const std::string& topic, const ARGUMENTS& arguments);

    /*!
     * Publish an event with both positional and keyword payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     * \param arguments The positional payload for the event.
     * \param kw_arguments The keyword payload for the event.
     */
    template <typename ARGUMENTS, typename KW_ARGUMENTS>
    void publish(const std::string& topic, const ARGUMENTS& arguments, const KW_ARGUMENTS& kw_arguments);

    /*!
     * Subscribe a handler to a topic to receive events.
     *
     * \param topic The URI of the topic to subscribe to.
     * \param handler The handler that will receive events under the subscription.
     * \return A future that resolves to a autobahn::subscription
     */
    boost::future<wamp_subscription> subscribe(
            const std::string& topic, const wamp_event_handler& handler);

    /*!
     * Unubscribe a handler to previosuly subscribed topic.
     *
     * \param subscription The subscription to unsubscribe from.
     * \return A future that synchronizes to the unsubscribed response.
     */
    boost::future<void> unsubscribe(const wamp_subscription& subscription);

    /*!
     * Calls a remote procedure with no arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \return A future that resolves to the result of the remote procedure call.
     */
    boost::future<wamp_call_result> call(const std::string& procedure);

    /*!
     * Calls a remote procedure with positional arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \param arguments The positional arguments for the call.
     * \return A future that resolves to the result of the remote procedure call.
     */
    template <typename ARGUMENTS>
    boost::future<wamp_call_result> call(
            const std::string& procedure, const ARGUMENTS& arguments);

    /*!
     * Calls a remote procedure with positional and keyword arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \param arguments The positional arguments for the call.
     * \param kw_arguments The keyword arguments for the call.
     * \return A future that resolves to the result of the remote procedure call.
     */
    template<typename ARGUMENTS, typename KW_ARGUMENTS>
    boost::future<wamp_call_result> call(
            const std::string& procedure, const ARGUMENTS& args, const KW_ARGUMENTS& kwargs);

    /*!
     * Register an procedure as a procedure that can be called remotely.
     *
     * \param uri The URI under which the procedure is to be exposed.
     * \param procedure The procedure to be exposed as a remotely callable procedure.
     * \param options Options when registering a procedure.
     * \return A future that resolves to a autobahn::registration
     */
    boost::future<wamp_registration> provide(
            const std::string& uri,
            const wamp_procedure& procedure,
            const provide_options& options = provide_options());

private:

    /// Process a WAMP ERROR message.
    void process_error(const wamp_message& message);

    /// Process a WAMP HELLO message.
    void process_welcome(const wamp_message& message);

    /// Process a WAMP RESULT message.
    void process_call_result(
            const wamp_message& message,
            msgpack::unique_ptr<msgpack::zone>&& zone);

    /// Process a WAMP SUBSCRIBED message.
    void process_subscribed(const wamp_message& message);

    /// Process a WAMP UNSUBSCRIBED message.
    void process_unsubscribed(const wamp_message& message);

    /// Process a WAMP EVENT message.
    void process_event(const wamp_message& message);

    /// Process a WAMP REGISTERED message.
    void process_registered(const wamp_message& message);

    /// Process a WAMP INVOCATION message.
    void process_invocation(const wamp_message& message);

    /// Process a WAMP GOODBYE message.
    void process_goodbye(const wamp_message& message);

    /// Send out message serialized in serialization buffer to ostream.
    void send();

    /// Receive one message from istream in m_unpacker.
    void receive_message();

    void got_handshake_reply(const boost::system::error_code& error);

    void got_message_header(const boost::system::error_code& error);

    void got_message_body(const boost::system::error_code& error);

    void got_message(const msgpack::object& object, msgpack::unique_ptr<msgpack::zone>&& zone);


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

    /// Map of outstanding WAMP unsubscribe requests (request ID -> unsubscribe request).
    std::map<uint64_t, wamp_unsubscribe_request> m_unsubscribe_requests;

    /// Map of subscribed handlers (subscription ID -> handler)
    std::multimap<uint64_t, wamp_event_handler> m_subscription_handlers;


    //////////////////////////////////////////////////////////////////////////////////////
    /// Callee

    /// Map of outstanding WAMP register requests (request ID -> register request).
    std::map<uint64_t, std::shared_ptr<wamp_register_request>> m_register_requests;

    /// Map of registered procedures (registration ID -> procedure)
    std::map<uint64_t, wamp_procedure> m_procedures;

    /// An unserialized, raw WAMP message.
    wamp_message m_message;
};

} // namespace autobahn

#include "wamp_session.ipp"

#endif // AUTOBAHN_SESSION_HPP
