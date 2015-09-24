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

#ifndef AUTOBAHN_WAMP_SESSION_HPP
#define AUTOBAHN_WAMP_SESSION_HPP

#include "wamp_call_options.hpp"
#include "wamp_call_result.hpp"
#include "wamp_event_handler.hpp"
#include "wamp_procedure.hpp"
#include "wamp_transport_handler.hpp"

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
class wamp_message;
class wamp_register_request;
class wamp_registration;
class wamp_subscribe_request;
class wamp_subscription;
class wamp_transport;
class wamp_unsubscribe_request;

/// Representation of a WAMP session.
class wamp_session :
        public wamp_transport_handler,
        public std::enable_shared_from_this<wamp_session>
{
public:

    /*!
     * Create a new WAMP session.
     *
     * \param io_service The io service to drive event dispatching.
     * \param in The inbound message based transport for this session.
     * \param out Tte output message based transport for this session.
     */
    wamp_session(
            boost::asio::io_service& io_service,
            bool debug_enabled=false);

    ~wamp_session();

    /*!
     * Establishes an initial connection to the router.
     *
     * \return A future that indicates if the session was successfully started.
     */
    boost::future<void> start();

    /*!
     * Closes the connection to the router.
     *
     * \return A future that indicates if the session was successfully stopped.
     */
    boost::future<void> stop();

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
    boost::future<std::string> leave(
            const std::string& reason=std::string("wamp.error.close_realm"));

    /*!
     * Publish an event with empty payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     */
    boost::future<void> publish(const std::string& topic);

    /*!
     * Publish an event with positional payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     * \param arguments The positional payload for the event.
     */
    template <typename List>
    boost::future<void> publish(
            const std::string& topic,
            const List& arguments);

    /*!
     * Publish an event with both positional and keyword payload to a topic.
     *
     * \param topic The URI of the topic to publish to.
     * \param arguments The positional payload for the event.
     * \param kw_arguments The keyword payload for the event.
     */
    template <typename List, typename Map>
    boost::future<void> publish(
            const std::string& topic,
            const List& arguments,
            const Map& kw_arguments);

    /*!
     * Subscribe a handler to a topic to receive events.
     *
     * \param topic The URI of the topic to subscribe to.
     * \param handler The handler that will receive events under the subscription.
     * \return A future that resolves to a autobahn::subscription
     */
    boost::future<wamp_subscription> subscribe(
            const std::string& topic,
            const wamp_event_handler& handler);

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
     * \param options The options to pass in the call to the router.
     * \return A future that resolves to the result of the remote procedure call.
     */
    boost::future<wamp_call_result> call(
            const std::string& procedure,
            const wamp_call_options& options = wamp_call_options());

    /*!
     * Calls a remote procedure with positional arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \param arguments The positional arguments for the call.
     * \param options The options to pass in the call to the router.
     * \return A future that resolves to the result of the remote procedure call.
     */
    template <typename List>
    boost::future<wamp_call_result> call(
            const std::string& procedure,
            const List& arguments,
            const wamp_call_options& options = wamp_call_options());

    /*!
     * Calls a remote procedure with positional and keyword arguments.
     *
     * \param procedure The URI of the remote procedure to call.
     * \param arguments The positional arguments for the call.
     * \param kw_arguments The keyword arguments for the call.
     * \param options The options to pass in the call to the router.
     * \return A future that resolves to the result of the remote procedure call.
     */
    template<typename List, typename Map>
    boost::future<wamp_call_result> call(
            const std::string& procedure, const List& arguments, const Map& kw_arguments,
            const wamp_call_options& options = wamp_call_options());

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
    virtual void on_attach(const std::shared_ptr<wamp_transport>& transport) override;
    virtual void on_detach(bool was_clean, const std::string& reason) override;
    virtual void on_message(wamp_message&& message) override;

    void process_error(wamp_message&& message);
    void process_welcome(wamp_message&& message);
    void process_call_result(wamp_message&& message);
    void process_subscribed(wamp_message&& message);
    void process_unsubscribed(wamp_message&& message);
    void process_event(wamp_message&& message);
    void process_registered(wamp_message&& message);
    void process_invocation(wamp_message&& message);
    void process_goodbye(wamp_message&& message);

    void send(wamp_message&& message, bool session_established=true);

    bool m_debug_enabled;

    boost::asio::io_service& m_io_service;

    /// The transport this session runs on.
    std::shared_ptr<wamp_transport> m_transport;

    /// Last request ID of outgoing WAMP requests.
    std::atomic<uint64_t> m_request_id;

    /// Synchronization for dealing with starting the session
    boost::promise<void> m_session_start;

    /// WAMP session ID (if the session is joined to a realm).
    uint64_t m_session_id;

    /// Future to be fired when session was joined.
    boost::promise<uint64_t> m_session_join;

    bool m_goodbye_sent;

    boost::promise<std::string> m_session_leave;

    /// Set to true when the session is running.
    bool m_running;

    /// Synchronization for dealing with stopping the session
    boost::promise<void> m_session_stop;

    //////////////////////////////////////////////////////////////////////////////////////
    /// Caller

    /// Map of outstanding WAMP calls (request ID -> call).
    std::map<uint64_t, std::shared_ptr<wamp_call>> m_calls;


    //////////////////////////////////////////////////////////////////////////////////////
    /// Subscriber

    /// Map of outstanding WAMP subscribe requests (request ID -> subscribe request).
    std::map<uint64_t, std::shared_ptr<wamp_subscribe_request>> m_subscribe_requests;

    /// Map of outstanding WAMP unsubscribe requests (request ID -> unsubscribe request).
    std::map<uint64_t, std::shared_ptr<wamp_unsubscribe_request>> m_unsubscribe_requests;

    /// Map of subscribed handlers (subscription ID -> handler)
    std::multimap<uint64_t, wamp_event_handler> m_subscription_handlers;


    //////////////////////////////////////////////////////////////////////////////////////
    /// Callee

    /// Map of outstanding WAMP register requests (request ID -> register request).
    std::map<uint64_t, std::shared_ptr<wamp_register_request>> m_register_requests;

    /// Map of registered procedures (registration ID -> procedure)
    std::map<uint64_t, wamp_procedure> m_procedures;
};

} // namespace autobahn

#include "wamp_session.ipp"

#endif // AUTOBAHN_WAMP_SESSION_HPP
