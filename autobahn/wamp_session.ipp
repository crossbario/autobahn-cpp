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
#include "wamp_call_result.hpp"
#include "wamp_event.hpp"
#include "wamp_invocation.hpp"
#include "wamp_invocation_result.hpp"
#include "wamp_message_type.hpp"
#include "wamp_publication.hpp"
#include "wamp_registration.hpp"
#include "wamp_register_request.hpp"
#include "wamp_subscribe_request.hpp"
#include "wamp_subscription.hpp"
#include "wamp_unsubscribe_request.hpp"

#if !(defined(_WIN32) || defined(WIN32))
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdlib.h>

namespace autobahn {

template<typename IStream, typename OStream>
wamp_session<IStream, OStream>::wamp_session(boost::asio::io_service& io, IStream& in, OStream& out, bool debug)
    : m_debug(debug)
    , m_stopped(false)
    , m_io(io)
    , m_in(in)
    , m_out(out)
    , m_packer(&m_buffer)
    , m_session_id(0)
    , m_request_id(0)
{
}

template<typename IStream, typename OStream>
boost::future<bool> wamp_session<IStream, OStream>::start()
{
    // Send the initial handshake packet informing the server which
    // serialization format we wish to use, and our maximum message size
    m_buffer_message_length[0] = 0x7F; // magic byte
    m_buffer_message_length[1] = 0xF2; // we are ready to receive messages up to 2**24 octets and encoded using MsgPack
    m_buffer_message_length[2] = 0x00; // reserved
    m_buffer_message_length[3] = 0x00; // reserved
    boost::asio::write(m_out, boost::asio::buffer(m_buffer_message_length, sizeof(m_buffer_message_length)));

    // Read the 4-byte reply from the server
    boost::asio::async_read(
        m_in,
        boost::asio::buffer(m_buffer_message_length, sizeof(m_buffer_message_length)),
        bind(&wamp_session<IStream, OStream>::got_handshake_reply, this, boost::asio::placeholders::error)
    );

    return m_handshake.get_future();
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::got_handshake_reply(const boost::system::error_code& error)
{
    // If there is an error trying to receive the handshake reply then set
    // the handshake promise to false to indicate that the session could
    // not be started.
    if (error) {
        if (m_debug) {
            std::cerr << "RawSocket handshake system error: " << error << std::endl;
        }

        m_handshake.set_value(false);
        return;
    }

    if (m_debug) {
        std::cerr << "RawSocket handshake reply received" << std::endl;
    }

    if (m_buffer_message_length[0] != 0x7F) {
        m_handshake.set_value(false);
        throw protocol_error("invalid magic byte in RawSocket handshake response");
    }
    if (((m_buffer_message_length[1] & 0x0F) != 0x02)) {
        m_handshake.set_value(false);
        // FIXME: this isn't exactly a "protocol error" => invent new exception
        throw protocol_error("RawSocket handshake reply: server does not speak MsgPack encoding");
    }
    if (m_debug) {
        std::cerr << "RawSocket handshake reply is valid: start WAMP message send-receive loop" << std::endl;
    }

    // It is now safe to try and join the session so set the handshake promise
    // indicating that the session has been started.
    m_handshake.set_value(true);

    // enter WAMP message send-receive ..
    //
    receive_message();
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::stop()
{
    m_stopped = true;
    try {
        m_in.close();
    } catch (...) {
    }
    try {
        m_out.close();
    } catch (...) {
    }
}

template<typename IStream, typename OStream>
boost::future<uint64_t> wamp_session<IStream, OStream>::join(const std::string& realm)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io.dispatch([this, weak_self, realm]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        // [HELLO, Realm|uri, Details|dict]
        m_packer.pack_array(3);

        m_packer.pack(static_cast<int> (message_type::HELLO));
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
    });

    return m_session_join.get_future();
}

template<typename IStream, typename OStream>
boost::future<std::string> wamp_session<IStream, OStream>::leave(const std::string& reason)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io.dispatch([this, weak_self, reason]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        m_goodbye_sent = true;
        m_session_id = 0;

        // [GOODBYE, Details|dict, Reason|uri]
        m_packer.pack_array(3);
        m_packer.pack(static_cast<int>(message_type::GOODBYE));
        m_packer.pack_map(0);
        m_packer.pack(reason);
        send();
    });

    return m_session_leave.get_future();
}

template<typename IStream, typename OStream>
boost::future<wamp_subscription> wamp_session<IStream, OStream>::subscribe(
        const std::string& topic, const wamp_event_handler& handler)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto subscribe_request = std::make_shared<wamp_subscribe_request>(handler);

    m_io.dispatch([this, weak_self, subscribe_request, topic]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [SUBSCRIBE, Request|id, Options|dict, Topic|uri]
        m_packer.pack_array(4);
        m_packer.pack(static_cast<int>(message_type::SUBSCRIBE));
        m_packer.pack(++m_request_id);
        m_packer.pack_map(0);
        m_packer.pack(topic);

        send();

        m_subscribe_requests.emplace(m_request_id, subscribe_request);
    });

    return subscribe_request->response().get_future();
}

template<typename IStream, typename OStream>
boost::future<void> wamp_session<IStream, OStream>::unsubscribe(const wamp_subscription& subscription)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto unsubscribe_request = std::make_shared<wamp_unsubscribe_request>();

    m_io.dispatch([this, weak_self, unsubscribe_request, subscription]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [UNSUBSCRIBE, Request|id, SUBSCRIBED.Subscription|id]
        m_packer.pack_array(3);
        m_packer.pack(static_cast<int>(message_type::UNSUBSCRIBE));
        m_packer.pack(++m_request_id);
        m_packer.pack(subscription.id());

        send();

        m_unsubscribe_requests.emplace(m_request_id, unsubscribe_request);
    });

    return unsubscribe_request->response().get_future();
}

template<typename IStream, typename OStream>
boost::future<wamp_registration> wamp_session<IStream, OStream>::provide(
        const std::string& name, const wamp_procedure& procedure, const provide_options& options)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto register_request = std::make_shared<wamp_register_request>(procedure);

    m_io.dispatch([this, weak_self, register_request, name, options]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [REGISTER, Request|id, Options|dict, Procedure|uri]
        m_packer.pack_array(4);
        m_packer.pack(static_cast<int>(message_type::REGISTER));
        m_packer.pack(++m_request_id);
        m_packer.pack(options);
        m_packer.pack(name);
        send();

        m_register_requests.emplace(m_request_id, register_request);
    });

    return register_request->response().get_future();
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::publish(const std::string& topic)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io.dispatch([this, weak_self, topic]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [PUBLISH, Request|id, Options|dict, Topic|uri]
        m_packer.pack_array(4);
        m_packer.pack(static_cast<int>(message_type::PUBLISH));
        m_packer.pack(++m_request_id);
        m_packer.pack_map(0);
        m_packer.pack(topic);
        send();
    });
}

template<typename IStream, typename OStream>
template <typename ARGUMENTS>
void wamp_session<IStream, OStream>::publish(const std::string& topic, const ARGUMENTS& arguments)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io.dispatch([this, weak_self, topic, arguments]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [PUBLISH, Request|id, Options|dict, Topic|uri, Arguments|list]
        m_packer.pack_array(5);
        m_packer.pack(static_cast<int>(message_type::PUBLISH));
        m_packer.pack(++m_request_id);
        m_packer.pack_map(0);
        m_packer.pack(topic);
        m_packer.pack(arguments);
        send();
    });
}

template<typename IStream, typename OStream>
template <typename ARGUMENTS, typename KW_ARGUMENTS>
void wamp_session<IStream, OStream>::publish(
        const std::string& topic, const ARGUMENTS& arguments, const KW_ARGUMENTS& kw_arguments)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());

    m_io.dispatch([this, weak_self, topic, arguments, kw_arguments]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [PUBLISH, Request|id, Options|dict, Topic|uri, Arguments|list, ArgumentsKw|dict]
        m_packer.pack_array(6);
        m_packer.pack(static_cast<int>(message_type::PUBLISH));
        m_packer.pack(++m_request_id);
        m_packer.pack_map(0);
        m_packer.pack(topic);
        m_packer.pack(arguments);
        m_packer.pack(kw_arguments);
        send();
    });
}

template<typename IStream, typename OStream>
boost::future<wamp_call_result> wamp_session<IStream, OStream>::call(const std::string& procedure)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto call = std::make_shared<wamp_call>();

    m_io.dispatch([this, weak_self, call, procedure]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [CALL, Request|id, Options|dict, Procedure|uri]
        m_packer.pack_array(4);
        m_packer.pack(static_cast<int>(message_type::CALL));
        m_packer.pack(++m_request_id);
        m_packer.pack_map(0);
        m_packer.pack(procedure);
        send();

        m_calls.emplace(m_request_id, call);
    });

    return call->result().get_future();
}

template<typename IStream, typename OStream>
template<typename ARGUMENTS>
boost::future<wamp_call_result> wamp_session<IStream, OStream>::call(
        const std::string& procedure, const ARGUMENTS& arguments)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto call = std::make_shared<wamp_call>();

    m_io.dispatch([this, weak_self, call, procedure, arguments]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [CALL, Request|id, Options|dict, Procedure|uri, Arguments|list]
        m_packer.pack_array(5);
        m_packer.pack(static_cast<int>(message_type::CALL));
        m_packer.pack(++m_request_id);
        m_packer.pack_map(0);
        m_packer.pack(procedure);
        m_packer.pack(arguments);
        send();

        m_calls.emplace(m_request_id, call);
    });

    return call->result().get_future();
}

template<typename IStream, typename OStream>
template<typename ARGUMENTS, typename KW_ARGUMENTS>
boost::future<wamp_call_result> wamp_session<IStream, OStream>::call(
        const std::string& procedure, const ARGUMENTS& arguments, const KW_ARGUMENTS& kw_arguments)
{
    auto weak_self = std::weak_ptr<wamp_session>(this->shared_from_this());
    auto call = std::make_shared<wamp_call>();

    m_io.dispatch([this, weak_self, call, procedure, arguments, kw_arguments]() {
        auto shared_self = weak_self.lock();
        if (!shared_self) {
            return;
        }

        if (!m_session_id) {
            throw no_session_error();
        }

        // [CALL, Request|id, Options|dict, Procedure|uri, Arguments|list, ArgumentsKw|dict]
        m_packer.pack_array(6);
        m_packer.pack(static_cast<int>(message_type::CALL));
        m_packer.pack(++m_request_id);
        m_packer.pack_map(0);
        m_packer.pack(procedure);
        m_packer.pack(arguments);
        m_packer.pack(kw_arguments);
        send();

        m_calls.emplace(m_request_id, call);
    });

    return call->result().get_future();
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_welcome(const wamp_message& message)
{
    m_session_id = message[1].as<uint64_t>();
    m_session_join.set_value(m_session_id);
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_goodbye(const wamp_message& message)
{
    m_session_id = 0;

    if (!m_goodbye_sent) {
        // if we did not initiate closing, reply ..

        // [GOODBYE, Details|dict, Reason|uri]
        m_packer.pack_array(3);
        m_packer.pack(static_cast<int>(message_type::GOODBYE));
        m_packer.pack_map(0);
        m_packer.pack(std::string("wamp.error.goodbye_and_out"));
        send();
    } else {
        // we previously initiated closing, so this
        // is the peer reply
    }

    std::string reason = message[2].as<std::string>();
    m_session_leave.set_value(reason);
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_error(const wamp_message& message)
{
    // [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri]
    // [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list]
    // [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list, ArgumentsKw|dict]

    // message length
    //
    if (message.size() != 5 && message.size() != 6 && message.size() != 7) {
        throw protocol_error("invalid ERROR message structure - length must be 5, 6 or 7");
    }

    // REQUEST.Type|int
    //
    if (message[1].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("invalid ERROR message structure - REQUEST.Type must be an integer");
    }
    message_type request_type = static_cast<message_type>(message[1].as<int>());

    if (request_type != message_type::CALL &&
         request_type != message_type::REGISTER &&
         request_type != message_type::UNREGISTER &&
         request_type != message_type::PUBLISH &&
         request_type != message_type::SUBSCRIBE &&
         request_type != message_type::UNSUBSCRIBE) {
        throw protocol_error("invalid ERROR message - ERROR.Type must one of CALL, REGISTER, UNREGISTER, SUBSCRIBE, UNSUBSCRIBE");
    }

    // REQUEST.Request|id
    if (message[2].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("invalid ERROR message structure - REQUEST.Request must be an integer");
    }
    uint64_t request_id = message[2].as<uint64_t>();

    // Details
    if (message[3].type != msgpack::type::MAP) {
        throw protocol_error("invalid ERROR message structure - Details must be a dictionary");
    }

    // Error|uri
    if (message[4].type != msgpack::type::STR) {
        throw protocol_error("invalid ERROR message - Error must be a string (URI)");
    }
    std::string error = message[4].as<std::string>();

    // Arguments|list
    if (message.size() > 5) {
        if (message[5].type  != msgpack::type::ARRAY) {
            throw protocol_error("invalid ERROR message structure - Arguments must be a list");
        }
    }

    // ArgumentsKw|list
    if (message.size() > 6) {
        if (message[6].type  != msgpack::type::MAP) {
            throw protocol_error("invalid ERROR message structure - ArgumentsKw must be a dictionary");
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

                    // FIXME: forward all error info .. also not sure if this is the correct
                    // way to use set_exception()
                    call_itr->second->result().set_exception(boost::copy_exception(std::runtime_error(error)));

                } else {
                    throw protocol_error("bogus ERROR message for non-pending CALL request ID");
                }
            }
            break;

        // FIXME: handle other error messages
        default:
            // TODO: We should at least assert or throw here.
            std::cerr << "unhandled ERROR message" << std::endl;
    }
}


template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_invocation(const wamp_message& message)
{
    // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict]
    // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list]
    // [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list, CALL.ArgumentsKw|dict]

    if (message.size() != 4 && message.size() != 5 && message.size() != 6) {
        throw protocol_error("INVOCATION message length must be 4, 5 or 6");
    }

    if (message[1].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("INVOCATION.Request must be an integer");
    }
    uint64_t request_id = message[1].as<uint64_t>();

    if (message[2].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("INVOCATION.Registration must be an integer");
    }
    uint64_t registration_id = message[2].as<uint64_t>();

    auto procedure_itr = m_procedures.find(registration_id);
    if (procedure_itr != m_procedures.end()) {
        if (message[3].type != msgpack::type::MAP) {
            throw protocol_error("INVOCATION.Details must be a map");
        }

        wamp_invocation invocation;
        if (message.size() > 4) {
            if (message[4].type != msgpack::type::ARRAY) {
                throw protocol_error("INVOCATION.Arguments must be an array/vector");
            }
            invocation.set_arguments(message[4]);

            if (message.size() > 5) {
                if (message[5].type != msgpack::type::MAP) {
                    throw protocol_error("INVOCATION.KwArguments must be a map");
                }
                invocation.set_kw_arguments(message[5]);
            }
        }

        // [YIELD, INVOCATION.Request|id, Options|dict]
        // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list]
        // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list, ArgumentsKw|dict]
        try {
            if (m_debug) {
                std::cerr << "Invoking procedure registered under " << registration_id << std::endl;
            }
            procedure_itr->second(invocation);

            m_packer.pack_array(4);
            m_packer.pack(static_cast<int>(message_type::YIELD));
            m_packer.pack(request_id);
            m_packer.pack_map(0);

            auto& result = invocation.result();
            if (result.arguments().type != msgpack::type::NIL) {
                m_packer.pack(result.arguments());
                if (result.kw_arguments().type != msgpack::type::NIL) {
                    m_packer.pack(result.kw_arguments());
                }
            } else if (result.kw_arguments().type != msgpack::type::NIL) {
                m_packer.pack(EMPTY_ARGUMENTS);
                m_packer.pack(result.kw_arguments());
            }
            send();
        }

        // [ERROR, INVOCATION, INVOCATION.Request|id, Details|dict, Error|uri]
        // [ERROR, INVOCATION, INVOCATION.Request|id, Details|dict, Error|uri, Arguments|list]
        // [ERROR, INVOCATION, INVOCATION.Request|id, Details|dict, Error|uri, Arguments|list, ArgumentsKw|dict]

        // FIXME: implement Autobahn-specific exception with error URI
        catch (const std::exception& e) {
            // we can at least describe the error with e.what()
            //
            m_packer.pack_array(7);
            m_packer.pack(static_cast<int>(message_type::ERROR));
            m_packer.pack(static_cast<int>(message_type::INVOCATION));
            m_packer.pack(request_id);
            m_packer.pack_map(0);
            m_packer.pack(std::string("wamp.error.runtime_error"));
            m_packer.pack_array(0);

            m_packer.pack_map(1);

            m_packer.pack(std::string("what"));
            m_packer.pack(std::string(e.what()));

            send();
        }
        catch (...) {
            // no information available on actual error
            //
            m_packer.pack_array(5);
            m_packer.pack(static_cast<int>(message_type::ERROR));
            m_packer.pack(static_cast<int>(message_type::INVOCATION));
            m_packer.pack(request_id);
            m_packer.pack_map(0);
            m_packer.pack(std::string("wamp.error.runtime_error"));
            send();
        }
    } else {
        throw protocol_error("bogus INVOCATION message for non-registered registration ID");
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_call_result(
        const wamp_message& message, msgpack::unique_ptr<msgpack::zone>&& zone)
{
    // [RESULT, CALL.Request|id, Details|dict]
    // [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list]
    // [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list, YIELD.ArgumentsKw|dict]

    if (message.size() != 3 && message.size() != 4 && message.size() != 5) {
        throw protocol_error("RESULT - length must be 3, 4 or 5");
    }

    if (message[1].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("RESULT - CALL.Request must be an id");
    }

    uint64_t request_id = message[1].as<uint64_t>();
    auto call_itr = m_calls.find(request_id);
    if (call_itr != m_calls.end()) {
        if (message[2].type != msgpack::type::MAP) {
            throw protocol_error("RESULT - Details must be a dictionary");
        }

        wamp_call_result result(std::move(zone));
        if (message.size() > 3) {
            if (message[3].type != msgpack::type::ARRAY) {
                throw protocol_error("RESULT - YIELD.Arguments must be a list");
            }
            result.set_arguments(message[3]);

            if (message.size() > 4) {
                if (message[4].type != msgpack::type::MAP) {
                    throw protocol_error("RESULT - YIELD.ArgumentsKw must be a dictionary");
                }
                result.set_kw_arguments(message[4]);
            }
        }
        call_itr->second->set_result(std::move(result));
    } else {
        throw protocol_error("bogus RESULT message for non-pending request ID");
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_subscribed(const wamp_message& message)
{
    // [SUBSCRIBED, SUBSCRIBE.Request|id, Subscription|id]
    if (message.size() != 3) {
        throw protocol_error("SUBSCRIBED - length must be 3");
    }

    if (message[1].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("SUBSCRIBED - SUBSCRIBED.Request must be an integer");
    }

    uint64_t request_id = message[1].as<uint64_t>();
    auto subscribe_request_itr = m_subscribe_requests.find(request_id);
    if (subscribe_request_itr != m_subscribe_requests.end()) {
        if (message[2].type != msgpack::type::POSITIVE_INTEGER) {
            throw protocol_error("SUBSCRIBED - SUBSCRIBED.Subscription must be an integer");
        }

        uint64_t subscription_id = message[2].as<uint64_t>();
        m_subscription_handlers.insert(std::make_pair(subscription_id, subscribe_request_itr->second->handler()));
        subscribe_request_itr->second->set_response(wamp_subscription(subscription_id));
        m_subscribe_requests.erase(request_id);
    } else {
        throw protocol_error("SUBSCRIBED - no pending request ID");
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_unsubscribed(const wamp_message& message)
{
    // [UNSUBSCRIBED, UNSUBSCRIBE.Request|id]
    if (message.size() != 2) {
        throw protocol_error("UNSUBSCRIBED - length must be 2");
    }

    if (message[1].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("UNSUBSCRIBED - UNSUBSCRIBED.Request must be an integer");
    }

    uint64_t request_id = message[1].as<uint64_t>();
    auto unsubscribe_request_itr = m_unsubscribe_requests.find(request_id);
    if (unsubscribe_request_itr != m_unsubscribe_requests.end()) {
        unsubscribe_request_itr->second->set_response();
        m_unsubscribe_requests.erase(request_id);
    } else {
        throw protocol_error("UNSUBSCRIBED - no pending request ID");
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_event(const wamp_message& message)
{
    // [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict]
    // [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list]
    // [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list, PUBLISH.ArgumentsKw|dict]

    if (message.size() != 4 && message.size() != 5 && message.size() != 6) {
        throw protocol_error("EVENT - length must be 4, 5 or 6");
    }

    if (message[1].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("EVENT - SUBSCRIBED.Subscription must be an integer");
    }

    uint64_t subscription_id = message[1].as<uint64_t>();

    auto subscription_handlers_itr = m_subscription_handlers.lower_bound(subscription_id);
    auto subscription_handlers_end = m_subscription_handlers.upper_bound(subscription_id);

    if (subscription_handlers_itr != m_subscription_handlers.end() &&
            subscription_handlers_itr != subscription_handlers_end) {

        if (message[2].type != msgpack::type::POSITIVE_INTEGER) {
            throw protocol_error("EVENT - PUBLISHED.Publication must be an id");
        }

        //uint64_t publication_id = message[2].as<uint64_t>();

        if (message[3].type != msgpack::type::MAP) {
            throw protocol_error("EVENT - Details must be a dictionary");
        }

        wamp_event event;
        if (message.size() > 4) {
            if (message[4].type != msgpack::type::ARRAY) {
                throw protocol_error("EVENT - EVENT.Arguments must be a list");
            }
            event.set_arguments(message[4]);

            if (message.size() > 5) {
                if (message[5].type != msgpack::type::MAP) {
                    throw protocol_error("EVENT - EVENT.ArgumentsKw must be a dictionary");
                }
                event.set_kw_arguments(message[5]);
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
            if (m_debug) {
                std::cerr << "Warning: event handler threw exception" << std::endl;
            }
        }

    } else {
        // silently swallow EVENT for non-existent subscription IDs.
        // We may have just unsubscribed, the this EVENT might be have
        // already been in-flight.
        if (m_debug) {
            std::cerr << "EVENT - non-existent subscription ID " << subscription_id << std::endl;
        }
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::process_registered(const wamp_message& message)
{
    // [REGISTERED, REGISTER.Request|id, Registration|id]

    if (message.size() != 3) {
        throw protocol_error("REGISTERED - length must be 3");
    }

    if (message[1].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("REGISTERED - REGISTERED.Request must be an integer");
    }

    uint64_t request_id = message[1].as<uint64_t>();
    auto register_request_itr = m_register_requests.find(request_id);
    if (register_request_itr != m_register_requests.end()) {
        if (message[2].type != msgpack::type::POSITIVE_INTEGER) {
            throw protocol_error("REGISTERED - REGISTERED.Registration must be an integer");
        }

        uint64_t registration_id = message[2].as<uint64_t>();
        m_procedures[registration_id] = register_request_itr->second->procedure();
        register_request_itr->second->set_response(wamp_registration(registration_id));
    } else {
        throw protocol_error("REGISTERED - no pending request ID");
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::receive_message()
{
    if (m_debug) {
        std::cerr << "RX preparing to receive message .." << std::endl;
    }

    // read 4 octets msg length prefix ..
    boost::asio::async_read(m_in,
        boost::asio::buffer(m_buffer_message_length, sizeof(m_buffer_message_length)),
        bind(&wamp_session<IStream, OStream>::got_message_header, this, boost::asio::placeholders::error));
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::got_message_header(const boost::system::error_code& error)
{
    if (!error) {
        m_message_length = ntohl(*((uint32_t*) &m_buffer_message_length));

        if (m_debug) {
            std::cerr << "RX message (" << m_message_length << " octets) ..." << std::endl;
        }

        // read actual message
        m_unpacker.reserve_buffer(m_message_length);

        boost::asio::async_read(m_in,
            boost::asio::buffer(m_unpacker.buffer(), m_message_length),
            bind(&wamp_session<IStream, OStream>::got_message_body, this, boost::asio::placeholders::error));
    } else {
        // TODO: Well this is no good. The session will basically just become unresponsive
        // at this point as we will no longer be trying to asynchronously receive messages.
        // Perhaps we should just try and read the next header.
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::got_message_body(const boost::system::error_code& error)
{
    if (!error) {
        if (m_debug) {
            std::cerr << "RX message received." << std::endl;
        }

        m_unpacker.buffer_consumed(m_message_length);
        msgpack::unpacked result;

        while (m_unpacker.next(&result)) {
            msgpack::object obj(result.get());

            if (m_debug) {
                std::cerr << "RX WAMP message: " << obj << std::endl;
            }

            got_message(obj, std::move(result.zone()));
        }

        if (!m_stopped) {
            receive_message();
        }
    } else {
        // TODO: Well this is no good. The session will basically just become unresponsive
        // at this point as we will no longer be trying to asynchronously receive messages.
        // Perhaps we should just try and read the next header.
    }
}


template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::got_message(
        const msgpack::object& obj, msgpack::unique_ptr<msgpack::zone>&& zone)
{

    if (obj.type != msgpack::type::ARRAY) {
        throw protocol_error("invalid message structure - message is not an array");
    }

    wamp_message message;
    obj.convert(&message);

    if (message.size() < 1) {
        throw protocol_error("invalid message structure - missing message code");
    }

    if (message[0].type != msgpack::type::POSITIVE_INTEGER) {
        throw protocol_error("invalid message code type - not an integer");
    }

    message_type code = static_cast<message_type>(message[0].as<int>());

    switch (code) {
        case message_type::HELLO:
            throw protocol_error("received HELLO message unexpected for WAMP client roles");
        case message_type::WELCOME:
            process_welcome(message);
            break;
        case message_type::ABORT:
            // FIXME
            break;
        case message_type::CHALLENGE:
            throw protocol_error("received CHALLENGE message - not implemented");
        case message_type::AUTHENTICATE:
            throw protocol_error("received AUTHENTICATE message unexpected for WAMP client roles");
        case message_type::GOODBYE:
            process_goodbye(message);
            break;
        case message_type::HEARTBEAT:
            // FIXME
            break;
        case message_type::ERROR:
            process_error(message);
            break;
        case message_type::PUBLISH:
            throw protocol_error("received PUBLISH message unexpected for WAMP client roles");
        case message_type::PUBLISHED:
            // FIXME
            break;
        case message_type::SUBSCRIBE:
            throw protocol_error("received SUBSCRIBE message unexpected for WAMP client roles");
        case message_type::SUBSCRIBED:
            process_subscribed(message);
            break;
        case message_type::UNSUBSCRIBE:
            throw protocol_error("received UNSUBSCRIBE message unexpected for WAMP client roles");
        case message_type::UNSUBSCRIBED:
            process_unsubscribed(message);
            break;
        case message_type::EVENT:
            process_event(message);
            break;
        case message_type::CALL:
            throw protocol_error("received CALL message unexpected for WAMP client roles");
        case message_type::CANCEL:
            throw protocol_error("received CANCEL message unexpected for WAMP client roles");
        case message_type::RESULT:
            process_call_result(message, std::move(zone));
            break;
        case message_type::REGISTER:
            throw protocol_error("received REGISTER message unexpected for WAMP client roles");
        case message_type::REGISTERED:
            process_registered(message);
            break;
        case message_type::UNREGISTER:
            throw protocol_error("received UNREGISTER message unexpected for WAMP client roles");
        case message_type::UNREGISTERED:
            // FIXME
            break;
        case message_type::INVOCATION:
            process_invocation(message);
            break;
        case message_type::INTERRUPT:
            throw protocol_error("received INTERRUPT message - not implemented");
        case message_type::YIELD:
            throw protocol_error("received YIELD message unexpected for WAMP client roles");
    }
}

template<typename IStream, typename OStream>
void wamp_session<IStream, OStream>::send()
{
    if (!m_stopped) {
        if (m_debug) {
            std::cerr << "TX message (" << m_buffer.size() << " octets) ..." << std::endl;
        }

        // FIXME: rework this for queuing, async_write using gathered write
        //
        // boost::asio::write(m_out, std::vector<boost::asio::const_buffer>& out_vec, handler);

        // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference/const_buffer/const_buffer/overload2.html
        // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference/async_write/overload1.html

        std::size_t written = 0;

        // write message length prefix
        uint32_t len = htonl(m_buffer.size());
        written += boost::asio::write(m_out, boost::asio::buffer((char*) &len, sizeof(len)));

        // write actual serialized message
        written += boost::asio::write(m_out, boost::asio::buffer(m_buffer.data(), m_buffer.size()));

        if (m_debug) {
            std::cerr << "TX message sent (" << written << " / " << (sizeof(len) + m_buffer.size()) << " octets)" << std::endl;
        }
    } else {
        if (m_debug) {
            std::cerr << "TX message skipped since session stopped (" << m_buffer.size() << " octets)." << std::endl;
        }
    }

    // clear serialization buffer
    m_buffer.clear();
}

} // namespace autobahn
