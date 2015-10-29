///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2014 Tavendo GmbH
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include "exceptions.hpp"
#include "wamp_call.hpp"
#include "wamp_event.hpp"
#include "wamp_invocation.hpp"
#include "wamp_message.hpp"
#include "wamp_message_type.hpp"
#include "wamp_publication.hpp"
#include "wamp_registration.hpp"
#include "wamp_register_request.hpp"
#include "wamp_subscribe_request.hpp"
#include "wamp_subscription.hpp"
#include "wamp_transport.hpp"
#include "wamp_unsubscribe_request.hpp"

#if !(defined(_WIN32) || defined(WIN32))
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <boost/system/error_code.hpp>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdlib.h>

namespace autobahn {

inline wamp_session::wamp_session(
        boost::asio::io_service& io_service,
        bool debug_enabled)
    : m_debug_enabled(debug_enabled)
    , m_io_service(io_service)
    , m_transport()
    , m_request_id(ATOMIC_VAR_INIT(0))
    , m_session_id(0)
    , m_goodbye_sent(false)
    , m_running(false)
{
}

inline wamp_session::~wamp_session()
{
}

inline boost::future<void> wamp_session::start()
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (m_running) {
            m_session_start.set_exception(protocol_error("session already started"));
            return;
        }

        if (!m_transport) {
            m_session_start.set_exception(no_transport_error());
            return;
        }

        m_running = true;
        m_session_start.set_value();
    });

    return m_session_start.get_future();
}

inline boost::future<void> wamp_session::stop()
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_running) {
            m_session_stop.set_exception(protocol_error("session already stopped"));
            return;
        }

        if (!m_transport) {
            m_session_start.set_exception(no_transport_error());
            return;
        }

        if (m_session_id) {
            m_session_stop.set_exception(protocol_error("session still joined"));
            return;
        }

        m_running = false;
        m_session_stop.set_value();
    });

    return m_session_stop.get_future();
}

inline boost::future<uint64_t> wamp_session::join(const std::string& realm)
{
    msgpack::zone zone;
    std::unordered_map<std::string, msgpack::object> roles;

    std::unordered_map<std::string, bool> caller_features;
    caller_features["call_timeout"] = true;
    std::unordered_map<std::string, msgpack::object> caller;
    caller["features"] = msgpack::object(caller_features, zone);
    roles["caller"] = msgpack::object(caller, zone);

    std::unordered_map<std::string, bool> callee_features;
    callee_features["call_timeout"] = true;
    std::unordered_map<std::string, msgpack::object> callee;
    callee["features"] = msgpack::object(callee_features, zone);
    roles["callee"] = msgpack::object(callee, zone);

    std::unordered_map<std::string, msgpack::object> publisher;
    roles["publisher"] = msgpack::object(publisher, zone);

    std::unordered_map<std::string, msgpack::object> subscriber;
    roles["subscriber"] = msgpack::object(subscriber, zone);

    std::unordered_map<std::string, msgpack::object> details;
    details["roles"] = msgpack::object(roles, zone);

    auto message = std::make_shared<wamp_message>(3, std::move(zone));
    message->set_field(0, static_cast<int>(message_type::HELLO));
    message->set_field(1, realm);
    message->set_field(2, details);

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (m_session_id) {
            m_session_join.set_exception(protocol_error("session already joined"));
            return;
        }

        try {
            send(std::move(*message), false);
        } catch (const std::exception& e) {
            m_session_join.set_exception(boost::copy_exception(e));
        }
    });

    return m_session_join.get_future();
}

inline boost::future<std::string> wamp_session::leave(const std::string& reason)
{
    auto message = std::make_shared<wamp_message>(3);
    message->set_field(0, static_cast<int>(message_type::GOODBYE));
    message->set_field(1, std::unordered_map<int, int>() /* No Details */);
    message->set_field(2, reason);

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (m_goodbye_sent) {
            m_session_leave.set_exception(protocol_error("goodbye already sent"));
        }

        try {
            send(std::move(*message), false);
            m_goodbye_sent = true;
        } catch (const std::exception& e) {
            m_session_leave.set_exception(boost::copy_exception(e));
        }

        m_session_id = 0;
    });

    return m_session_leave.get_future();
}

inline boost::future<void> wamp_session::publish(const std::string& topic)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(4);
    message->set_field(0, static_cast<int>(message_type::PUBLISH));
    message->set_field(1, request_id);
    message->set_field(2, std::unordered_map<int, int>() /* No Options */);
    message->set_field(3, topic);

    auto result = std::make_shared<boost::promise<void>>();
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            result->set_value();
        } catch (const std::exception& e) {
            result->set_exception(boost::copy_exception(e));
        }
    });

    return result->get_future();
}

template <typename List>
inline boost::future<void> wamp_session::publish(const std::string& topic, const List& arguments)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(5);
    message->set_field(0, static_cast<int>(message_type::PUBLISH));
    message->set_field(1, request_id);
    message->set_field(2, std::unordered_map<int, int>() /* No Options */);
    message->set_field(3, topic);
    message->set_field(4, arguments);

    auto result = std::make_shared<boost::promise<void>>();
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            result->set_value();
        } catch (const std::exception& e) {
            result->set_exception(boost::copy_exception(e));
        }
    });

    return result->get_future();
}

template <typename List, typename Map>
inline boost::future<void> wamp_session::publish(
        const std::string& topic, const List& arguments, const Map& kw_arguments)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(6);
    message->set_field(0, static_cast<int>(message_type::PUBLISH));
    message->set_field(1, request_id);
    message->set_field(2, std::unordered_map<int, int>() /* No Options */);
    message->set_field(3, topic);
    message->set_field(4, arguments);
    message->set_field(5, kw_arguments);

    auto result = std::make_shared<boost::promise<void>>();
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            result->set_value();
        } catch (const std::exception& e) {
            result->set_exception(boost::copy_exception(e));
        }
    });

    return result->get_future();
}

inline boost::future<wamp_subscription> wamp_session::subscribe(
        const std::string& topic, const wamp_event_handler& handler)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(4);
    message->set_field(0, static_cast<int>(message_type::SUBSCRIBE));
    message->set_field(1, request_id);
    message->set_field(2, std::unordered_map<int, int>() /* No Options */);
    message->set_field(3, topic);

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto subscribe_request = std::make_shared<wamp_subscribe_request>(handler);

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            m_subscribe_requests.emplace(request_id, subscribe_request);
        } catch (const std::exception& e) {
            subscribe_request->response().set_exception(boost::copy_exception(e));
        }
    });

    return subscribe_request->response().get_future();
}

inline boost::future<void> wamp_session::unsubscribe(const wamp_subscription& subscription)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(3);
    message->set_field(0, static_cast<int>(message_type::UNSUBSCRIBE));
    message->set_field(1, request_id);
    message->set_field(2, subscription.id());

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto unsubscribe_request = std::make_shared<wamp_unsubscribe_request>();

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            m_unsubscribe_requests.emplace(request_id, unsubscribe_request);
        } catch (const std::exception& e) {
            unsubscribe_request->response().set_exception(boost::copy_exception(e));
        }
    });

    return unsubscribe_request->response().get_future();
}

inline boost::future<wamp_call_result> wamp_session::call(
        const std::string& procedure,
        const wamp_call_options& options)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(4);
    message->set_field(0, static_cast<int>(message_type::CALL));
    message->set_field(1, request_id);
    message->set_field(2, options);
    message->set_field(3, procedure);

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto call = std::make_shared<wamp_call>();

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            m_calls.emplace(request_id, call);
        } catch (const std::exception& e) {
            call->result().set_exception(boost::copy_exception(e));
        }
    });

    return call->result().get_future();
}

template<typename List>
inline boost::future<wamp_call_result> wamp_session::call(
        const std::string& procedure,
        const List& arguments,
        const wamp_call_options& options)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(5);
    message->set_field(0, static_cast<int>(message_type::CALL));
    message->set_field(1, request_id);
    message->set_field(2, options);
    message->set_field(3, procedure);
    message->set_field(4, arguments);

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto call = std::make_shared<wamp_call>();

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            m_calls.emplace(request_id, call);
        } catch (const std::exception& e) {
            call->result().set_exception(boost::copy_exception(e));
        }
    });

    return call->result().get_future();
}

template<typename List, typename Map>
inline boost::future<wamp_call_result> wamp_session::call(
        const std::string& procedure,
        const List& arguments,
        const Map& kw_arguments,
        const wamp_call_options& options)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(6);
    message->set_field(0, static_cast<int>(message_type::CALL));
    message->set_field(1, request_id);
    message->set_field(2, options);
    message->set_field(3, procedure);
    message->set_field(4, arguments);
    message->set_field(5, kw_arguments);

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto call = std::make_shared<wamp_call>();

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            m_calls.emplace(request_id, call);
        } catch (const std::exception& e) {
            call->result().set_exception(boost::copy_exception(e));
        }
    });

    return call->result().get_future();
}

inline boost::future<wamp_registration> wamp_session::provide(
        const std::string& name,
        const wamp_procedure& procedure,
        const provide_options& options)
{
    uint64_t request_id = ++m_request_id;

    auto message = std::make_shared<wamp_message>(4);
    message->set_field(0, static_cast<int>(message_type::REGISTER));
    message->set_field(1, request_id);
    message->set_field(2, options);
    message->set_field(3, name);

    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto register_request = std::make_shared<wamp_register_request>(procedure);

    m_io_service.dispatch([=]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        try {
            send(std::move(*message));
            m_register_requests.emplace(request_id, register_request);
        } catch (const std::exception& e) {
            register_request->response().set_exception(boost::copy_exception(e));
        }
    });

    return register_request->response().get_future();
}

inline void wamp_session::on_attach(const std::shared_ptr<wamp_transport>& transport)
{
    // FIXME: We should be deferring this operation to the io service. This
    //        will almost certainly require us to return a future here to
    //        all the caller to sync up with the actual attaching of the
    //        transport.

    if (m_transport) {
        throw protocol_error("Transport already attached to session");
    }

    // This should never be possible as you cannot start a session without
    // having a transport already attached.
    assert(!m_running);

    m_transport = transport;
}

inline void wamp_session::on_detach(bool was_clean, const std::string& reason)
{
    // FIXME: We should be deferring this operation to the io service. This
    //        will almost certainly require us to return a future here to
    //        all the caller to sync up with the actual detaching of the
    //        transport.

    if (!m_transport) {
        throw protocol_error("Transport already detached from session");
    }

    // FIXME: Figure out what to do if we are detaching a transport
    //        from a session that is still running. Ideally we would
    //        not detach the transport until m_session_stop is satisfied.
    //        Perhaps we could use the same promise/future discussed above.
    //        One side effect here will be if the transport is re-used for
    //        another session as it may still receive messages for the old
    //        session.
    assert(!m_running);

    m_transport.reset();
}

inline void wamp_session::on_message(wamp_message&& message)
{
    // FIXME: Move this check into the transport
    //if (obj.type != msgpack::type::ARRAY) {
    //    throw protocol_error("invalid message structure - message is not an array");
    //}

    if (message.size() < 1) {
        throw protocol_error("invalid message structure - missing message code");
    }

    if (!message.is_field_type(0, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("invalid message code type - not an integer");
    }

    message_type code = static_cast<message_type>(message.field<int>(0));

    switch (code) {
        case message_type::HELLO:
            throw protocol_error("received HELLO message unexpected for WAMP client roles");
        case message_type::WELCOME:
            process_welcome(std::move(message));
            break;
        case message_type::ABORT:
            // FIXME
            break;
        case message_type::CHALLENGE:
            throw protocol_error("received CHALLENGE message - not implemented");
        case message_type::AUTHENTICATE:
            throw protocol_error("received AUTHENTICATE message unexpected for WAMP client roles");
        case message_type::GOODBYE:
            process_goodbye(std::move(message));
            break;
        case message_type::HEARTBEAT:
            // FIXME
            break;
        case message_type::ERROR:
            process_error(std::move(message));
            break;
        case message_type::PUBLISH:
            throw protocol_error("received PUBLISH message unexpected for WAMP client roles");
        case message_type::PUBLISHED:
            // FIXME
            break;
        case message_type::SUBSCRIBE:
            throw protocol_error("received SUBSCRIBE message unexpected for WAMP client roles");
        case message_type::SUBSCRIBED:
            process_subscribed(std::move(message));
            break;
        case message_type::UNSUBSCRIBE:
            throw protocol_error("received UNSUBSCRIBE message unexpected for WAMP client roles");
        case message_type::UNSUBSCRIBED:
            process_unsubscribed(std::move(message));
            break;
        case message_type::EVENT:
            process_event(std::move(message));
            break;
        case message_type::CALL:
            throw protocol_error("received CALL message unexpected for WAMP client roles");
        case message_type::CANCEL:
            throw protocol_error("received CANCEL message unexpected for WAMP client roles");
        case message_type::RESULT:
            process_call_result(std::move(message));
            break;
        case message_type::REGISTER:
            throw protocol_error("received REGISTER message unexpected for WAMP client roles");
        case message_type::REGISTERED:
            process_registered(std::move(message));
            break;
        case message_type::UNREGISTER:
            throw protocol_error("received UNREGISTER message unexpected for WAMP client roles");
        case message_type::UNREGISTERED:
            // FIXME
            break;
        case message_type::INVOCATION:
            process_invocation(std::move(message));
            break;
        case message_type::INTERRUPT:
            throw protocol_error("received INTERRUPT message - not implemented");
        case message_type::YIELD:
            throw protocol_error("received YIELD message unexpected for WAMP client roles");
    }
}

inline void wamp_session::process_welcome(wamp_message&& message)
{
    m_session_id = message.field<uint64_t>(1);
    m_session_join.set_value(m_session_id);
}

inline void wamp_session::process_goodbye(wamp_message&& message)
{
    m_session_id = 0;

    // if we did not initiate closing, reply ..
    if (!m_goodbye_sent) {
        // [GOODBYE, Details|dict, Reason|uri]
        wamp_message goodbye(3);
        goodbye.set_field(0, static_cast<int>(message_type::GOODBYE));
        goodbye.set_field(1, std::unordered_map<int,int>() /* No Details */);
        goodbye.set_field(2, std::string("wamp.error.goodbye_and_out"));

        send(std::move(goodbye));
    } else {
        // we previously initiated closing, so this
        // is the peer reply.
    }

    std::string reason = message.field<std::string>(2);
    m_session_leave.set_value(reason);
}

inline void wamp_session::process_error(wamp_message&& message)
{
    // [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri]
    // [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list]
    // [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list, ArgumentsKw|dict]

    if (message.size() < 5 || message.size() > 7) {
        throw protocol_error("invalid ERROR message structure - length must be 5, 6 or 7");
    }

    // REQUEST.Type|int
    //
    if (!message.is_field_type(1, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("invalid ERROR message structure - REQUEST.Type must be an integer");
    }
    auto request_type = static_cast<message_type>(message.field<int>(1));

    if (request_type != message_type::CALL &&
         request_type != message_type::REGISTER &&
         request_type != message_type::UNREGISTER &&
         request_type != message_type::PUBLISH &&
         request_type != message_type::SUBSCRIBE &&
         request_type != message_type::UNSUBSCRIBE) {
        throw protocol_error("invalid ERROR message - ERROR.Type must one of CALL, REGISTER, UNREGISTER, SUBSCRIBE, UNSUBSCRIBE");
    }

    // REQUEST.Request|id
    if (!message.is_field_type(2, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("invalid ERROR message structure - REQUEST.Request must be an integer");
    }
    auto request_id = message.field<uint64_t>(2);

    // Details
    if (!message.is_field_type(3, msgpack::type::MAP)) {
        throw protocol_error("invalid ERROR message structure - Details must be a dictionary");
    }

    // Error|uri
    if (!message.is_field_type(4, msgpack::type::STR)) {
        throw protocol_error("invalid ERROR message - Error must be a string (URI)");
    }
    auto error = std::move(message.field<std::string>(4));

    // Arguments|list
    if (message.size() > 5) {
        if (!message.is_field_type(5, msgpack::type::ARRAY)) {
            throw protocol_error("invalid ERROR message structure - Arguments must be a list");
        }
    }

    // ArgumentsKw|list
    if (message.size() > 6) {
        if (!message.is_field_type(6, msgpack::type::MAP)) {
            throw protocol_error("invalid ERROR message structure - ArgumentsKw must be a dictionary");
        }
        try {
            auto kw_args = message.field<std::unordered_map<std::string, std::string>>(6);
            const auto itr = kw_args.find("what");
            if (itr != kw_args.end()) {
                error += ": ";
                error += itr->second;
            }
        } catch (const std::exception& e) {
            if (m_debug_enabled) {
                std::cerr << "failed to parse error message keyword arguments" << std::endl;
            }

            error += ": unknown exception";
        }
    }

    switch (request_type) {

        case message_type::CALL:
            {
                //
                // process CALL ERROR
                //
                auto call_itr = m_calls.find(request_id);

                if (call_itr != m_calls.end()) {

                    // FIXME: Forward all error info.
                    call_itr->second->result().set_exception(std::runtime_error(error));

                } else {
                    throw protocol_error("bogus ERROR message for non-pending CALL request ID");
                }
            }
            break;

        // FIXME: handle other error messages
        default:
            throw protocol_error("unhandled ERROR message");
            break;
    }
}


inline void wamp_session::process_invocation(wamp_message&& message)
{
    // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict]
    // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list]
    // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list, CALL.ArgumentsKw|dict]

    if (message.size() < 4 || message.size() > 6) {
        throw protocol_error("INVOCATION message length must be 4, 5 or 6");
    }

    if (!message.is_field_type(1, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("INVOCATION.Request must be an integer");
    }
    uint64_t request_id = message.field<uint64_t>(1);

    if (!message.is_field_type(2, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("INVOCATION.Registration must be an integer");
    }
    uint64_t registration_id = message.field<uint64_t>(2);

    auto procedure_itr = m_procedures.find(registration_id);
    if (procedure_itr != m_procedures.end()) {
        if (!message.is_field_type(3, msgpack::type::MAP)) {
            throw protocol_error("INVOCATION.Details must be a map");
        }

        wamp_invocation invocation = std::make_shared<wamp_invocation_impl>();
        invocation->set_request_id(request_id);

        if (message.size() > 4) {
            if (!message.is_field_type(4, msgpack::type::ARRAY)) {
                throw protocol_error("INVOCATION.Arguments must be an array/vector");
            }
            invocation->set_arguments(message.field(4));

            if (message.size() > 5) {
                if (!message.is_field_type(5, msgpack::type::MAP)) {
                    throw protocol_error("INVOCATION.KwArguments must be a map");
                }
                invocation->set_kw_arguments(message.field(5));
            }
        }

        invocation->set_zone(std::move(message.zone()));

        auto weak_this = std::weak_ptr<wamp_session>(this->shared_from_this());

        auto send_result_fn = [weak_this] (const std::shared_ptr<wamp_message>& message) {
            // Make sure the session still exists, since the invocation could run
            // on a different thread.
            auto shared_this = weak_this.lock();
            if (!shared_this) {
                return; // FIXME: or throw exception?
            }

            // Send to the io_service thread, and make sure the session still exists (again).
            shared_this->m_io_service.dispatch([weak_this, message] {
                auto shared_this = weak_this.lock();
                if (!shared_this) {
                    return; // FIXME: or throw exception?
                }
                shared_this->send(std::move(*message));
            });
        };

        invocation->set_send_result_fn(std::move(send_result_fn));

        try {
            if (m_debug_enabled) {
                std::cerr << "Invoking procedure registered under " << registration_id << std::endl;
            }
            procedure_itr->second(invocation);
        }

        // FIXME: implement Autobahn-specific exception with error URI
        catch (const std::exception& e) {
            // we can at least describe the error with e.what()
            //
            if (invocation->sendable()) {
                std::map<std::string, std::string> error_kw_arguments;
                error_kw_arguments["what"] = e.what();
                invocation->error("wamp.error.runtime_error", EMPTY_ARGUMENTS, error_kw_arguments);
            }
        }
        catch (...) {
            // no information available on actual error
            //
            if (invocation->sendable()) {
                invocation->error("wamp.error.runtime_error");
            }
        }
    } else {
        throw protocol_error("bogus INVOCATION message for non-registered registration ID");
    }
}

inline void wamp_session::process_call_result(wamp_message&& message)
{
    // [RESULT, CALL.Request|id, Details|dict]
    // [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list]
    // [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list, YIELD.ArgumentsKw|dict]

    if (message.size() < 3 || message.size() > 5) {
        throw protocol_error("RESULT - length must be 3, 4 or 5");
    }

    if (!message.is_field_type(1, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("RESULT - CALL.Request must be an id");
    }
    uint64_t request_id = message.field<uint64_t>(1);

    auto call_itr = m_calls.find(request_id);
    if (call_itr != m_calls.end()) {
        if (!message.is_field_type(2, msgpack::type::MAP)) {
            throw protocol_error("RESULT - Details must be a dictionary");
        }

        wamp_call_result result(std::move(message.zone()));
        if (message.size() > 3) {
            if (!message.is_field_type(3, msgpack::type::ARRAY)) {
                throw protocol_error("RESULT - YIELD.Arguments must be a list");
            }
            result.set_arguments(message.field(3));

            if (message.size() > 4) {
                if (!message.is_field_type(4, msgpack::type::MAP)) {
                    throw protocol_error("RESULT - YIELD.ArgumentsKw must be a dictionary");
                }
                result.set_kw_arguments(message.field(4));
            }
        }
        call_itr->second->set_result(std::move(result));
    } else {
        throw protocol_error("bogus RESULT message for non-pending request ID");
    }
}

inline void wamp_session::process_subscribed(wamp_message&& message)
{
    // [SUBSCRIBED, SUBSCRIBE.Request|id, Subscription|id]
    if (message.size() != 3) {
        throw protocol_error("SUBSCRIBED - length must be 3");
    }

    if (!message.is_field_type(1, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("SUBSCRIBED - SUBSCRIBED.Request must be an integer");
    }
    uint64_t request_id = message.field<uint64_t>(1);

    auto subscribe_request_itr = m_subscribe_requests.find(request_id);
    if (subscribe_request_itr != m_subscribe_requests.end()) {
        if (!message.is_field_type(2, msgpack::type::POSITIVE_INTEGER)) {
            throw protocol_error("SUBSCRIBED - SUBSCRIBED.Subscription must be an integer");
        }

        uint64_t subscription_id = message.field<uint64_t>(2);
        m_subscription_handlers.insert(
                std::make_pair(subscription_id, subscribe_request_itr->second->handler()));
        subscribe_request_itr->second->set_response(wamp_subscription(subscription_id));
        m_subscribe_requests.erase(request_id);
    } else {
        throw protocol_error("SUBSCRIBED - no pending request ID");
    }
}

inline void wamp_session::process_unsubscribed(wamp_message&& message)
{
    // [UNSUBSCRIBED, UNSUBSCRIBE.Request|id]
    if (message.size() != 2) {
        throw protocol_error("UNSUBSCRIBED - length must be 2");
    }

    if (!message.is_field_type(1, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("UNSUBSCRIBED - UNSUBSCRIBED.Request must be an integer");
    }
    uint64_t request_id = message.field<uint64_t>(1);

    auto unsubscribe_request_itr = m_unsubscribe_requests.find(request_id);
    if (unsubscribe_request_itr != m_unsubscribe_requests.end()) {
        unsubscribe_request_itr->second->set_response();
        m_unsubscribe_requests.erase(request_id);
    } else {
        throw protocol_error("UNSUBSCRIBED - no pending request ID");
    }
}

inline void wamp_session::process_event(wamp_message&& message)
{
    // [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict]
    // [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list]
    // [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list, PUBLISH.ArgumentsKw|dict]

    if (message.size() < 4 || message.size() > 6) {
        throw protocol_error("EVENT - length must be 4, 5 or 6");
    }

    if (!message.is_field_type(1, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("EVENT - SUBSCRIBED.Subscription must be an integer");
    }
    uint64_t subscription_id = message.field<uint64_t>(1);

    auto subscription_handlers_itr = m_subscription_handlers.lower_bound(subscription_id);
    auto subscription_handlers_end = m_subscription_handlers.upper_bound(subscription_id);

    if (subscription_handlers_itr != m_subscription_handlers.end() &&
            subscription_handlers_itr != subscription_handlers_end) {

        if (!message.is_field_type(2, msgpack::type::POSITIVE_INTEGER)) {
            throw protocol_error("EVENT - PUBLISHED.Publication must be an id");
        }
        //uint64_t publication_id = message[2].as<uint64_t>();

        if (!message.is_field_type(3, msgpack::type::MAP)) {
            throw protocol_error("EVENT - Details must be a dictionary");
        }

        wamp_event event(std::move(message.zone()));
        if (message.size() > 4) {
            if (!message.is_field_type(4, msgpack::type::ARRAY)) {
                throw protocol_error("EVENT - EVENT.Arguments must be a list");
            }
            event.set_arguments(message.field(4));

            if (message.size() > 5) {
                if (!message.is_field_type(5, msgpack::type::MAP)) {
                    throw protocol_error("EVENT - EVENT.ArgumentsKw must be a dictionary");
                }
                event.set_kw_arguments(message.field(5));
            }
        }

        try {
            // now trigger the user supplied event handler ..
            //
            while (subscription_handlers_itr != subscription_handlers_end) {
                 (subscription_handlers_itr->second)(event);
                 ++subscription_handlers_itr;
            }
        } catch (...) {
            if (m_debug_enabled) {
                std::cerr << "Warning: event handler threw exception" << std::endl;
            }
        }

    } else {
        // silently swallow EVENT for non-existent subscription IDs.
        // We may have just unsubscribed, this EVENT might be have
        // already been in-flight.
        if (m_debug_enabled) {
            std::cerr << "EVENT - non-existent subscription ID " << subscription_id << std::endl;
        }
    }
}

inline void wamp_session::process_registered(wamp_message&& message)
{
    // [REGISTERED, REGISTER.Request|id, Registration|id]

    if (message.size() != 3) {
        throw protocol_error("REGISTERED - length must be 3");
    }

    if (!message.is_field_type(1, msgpack::type::POSITIVE_INTEGER)) {
        throw protocol_error("REGISTERED - REGISTERED.Request must be an integer");
    }
    uint64_t request_id = message.field<uint64_t>(1);

    auto register_request_itr = m_register_requests.find(request_id);
    if (register_request_itr != m_register_requests.end()) {
        if (!message.is_field_type(2, msgpack::type::POSITIVE_INTEGER)) {
            throw protocol_error("REGISTERED - REGISTERED.Registration must be an integer");
        }
        uint64_t registration_id = message.field<uint64_t>(2);

        m_procedures[registration_id] = register_request_itr->second->procedure();
        register_request_itr->second->set_response(wamp_registration(registration_id));
    } else {
        throw protocol_error("REGISTERED - no pending request ID");
    }
}

inline void wamp_session::send(wamp_message&& message, bool session_established)
{
    if (!m_running) {
        throw protocol_error("session not running");
    }

    if (!m_transport) {
        throw no_transport_error();
    }

    if (session_established && !m_session_id) {
        throw no_session_error();
    }

    m_transport->send(std::move(message));
}

} // namespace autobahn
