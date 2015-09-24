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

#include "wamp_message.hpp"
#include "wamp_message_type.hpp"

#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <tuple>

namespace autobahn {

inline wamp_invocation_impl::wamp_invocation_impl()
    : m_zone()
    , m_arguments(EMPTY_ARGUMENTS)
    , m_kw_arguments(EMPTY_KW_ARGUMENTS)
    , m_send_result_fn()
    , m_request_id(0)
{
}

inline std::size_t wamp_invocation_impl::number_of_arguments() const
{
    return m_arguments.type == msgpack::type::ARRAY ? m_arguments.via.array.size : 0;
}

inline std::size_t wamp_invocation_impl::number_of_kw_arguments() const
{
    return m_kw_arguments.type == msgpack::type::MAP ? m_kw_arguments.via.map.size : 0;
}

template <typename T>
inline T wamp_invocation_impl::argument(std::size_t index) const
{
    if (m_arguments.type != msgpack::type::ARRAY || m_arguments.via.array.size <= index) {
        throw std::out_of_range("no argument at index " + boost::lexical_cast<std::string>(index));
    }
    return m_arguments.via.array.ptr[index].as<T>();
}

template <typename List>
inline List wamp_invocation_impl::arguments() const
{
    return m_arguments.as<List>();
}

template <typename List>
inline void wamp_invocation_impl::get_arguments(List& args) const
{
    m_arguments.convert(args);
}

template <typename... T>
inline void wamp_invocation_impl::get_each_argument(T&... args) const
{
    auto args_tuple = std::make_tuple(std::ref(args)...);
    m_arguments.convert(args_tuple);
}

template <typename T>
inline T wamp_invocation_impl::kw_argument(const std::string& key) const
{
    if (m_kw_arguments.type != msgpack::type::MAP) {
        throw msgpack::type_error();
    }
    for (std::size_t i = 0; i < m_kw_arguments.via.map.size; ++i) {
        const msgpack::object_kv& kv = m_kw_arguments.via.map.ptr[i];
        if (kv.key.type == msgpack::type::STR && key.size() == kv.key.via.str.size
                && key.compare(0, key.size(), kv.key.via.str.ptr, kv.key.via.str.size) == 0)
        {
            return kv.val.as<T>();
        }
    }
    throw std::out_of_range(key + " keyword argument doesn't exist");
}

template <typename T>
inline T wamp_invocation_impl::kw_argument(const char* key) const
{
    if (m_kw_arguments.type != msgpack::type::MAP) {
        throw msgpack::type_error();
    }
    std::size_t key_size = strlen(key);
    for (std::size_t i = 0; i < m_kw_arguments.via.map.size; ++i) {
        const msgpack::object_kv& kv = m_kw_arguments.via.map.ptr[i];
        if (kv.key.type == msgpack::type::STR && key_size == kv.key.via.str.size
                && memcmp(key, kv.key.via.str.ptr, key_size) == 0)
        {
            return kv.val.as<T>();
        }
    }
    throw std::out_of_range(std::string(key) + " keyword argument doesn't exist");
}

template <typename T>
inline T wamp_invocation_impl::kw_argument_or(const std::string& key, const T& fallback) const
{
    if (m_kw_arguments.type != msgpack::type::MAP) {
        throw msgpack::type_error();
    }
    for (std::size_t i = 0; i < m_kw_arguments.via.map.size; ++i) {
        const msgpack::object_kv& kv = m_kw_arguments.via.map.ptr[i];
        if (kv.key.type == msgpack::type::STR && key.size() == kv.key.via.str.size
                && key.compare(0, key.size(), kv.key.via.str.ptr, kv.key.via.str.size) == 0)
        {
            return kv.val.as<T>();
        }
    }
    return fallback;
}

template <typename T>
inline T wamp_invocation_impl::kw_argument_or(const char* key, const T& fallback) const
{
    if (m_kw_arguments.type != msgpack::type::MAP) {
        throw msgpack::type_error();
    }
    std::size_t key_size = strlen(key);
    for (std::size_t i = 0; i < m_kw_arguments.via.map.size; ++i) {
        const msgpack::object_kv& kv = m_kw_arguments.via.map.ptr[i];
        if (kv.key.type == msgpack::type::STR && key_size == kv.key.via.str.size
                && memcmp(key, kv.key.via.str.ptr, key_size) == 0)
        {
            return kv.val.as<T>();
        }
    }
    throw fallback;
}

template <typename Map>
inline Map wamp_invocation_impl::kw_arguments() const
{
    return m_kw_arguments.as<Map>();
}

template <typename Map>
inline void wamp_invocation_impl::get_kw_arguments(Map& kw_args) const
{
    m_kw_arguments.convert(kw_args);
}

inline void wamp_invocation_impl::empty_result()
{
    throw_if_not_sendable();

    // [YIELD, INVOCATION.Request|id, Options|dict]
    auto message = std::make_shared<wamp_message>(3);
    message->set_field(0, static_cast<int>(message_type::YIELD));
    message->set_field(1, m_request_id);
    message->set_field(2, std::map<int, int>() /* No details */);

    m_send_result_fn(message);
    m_send_result_fn = send_result_fn();
}

template<typename List>
inline void wamp_invocation_impl::result(const List& arguments)
{
    throw_if_not_sendable();

    // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list]
    auto message = std::make_shared<wamp_message>(4);
    message->set_field(0, static_cast<int>(message_type::YIELD));
    message->set_field(1, m_request_id);
    message->set_field(2, std::map<int, int>() /* No details */);
    message->set_field(3, arguments);

    m_send_result_fn(message);
    m_send_result_fn = send_result_fn();
}

template<typename List, typename Map>
inline void wamp_invocation_impl::result(
        const List& arguments, const Map& kw_arguments)
{
    throw_if_not_sendable();

    // [YIELD, INVOCATION.Request|id, Options|dict, Arguments|list, ArgumentsKw|dict]
    auto message = std::make_shared<wamp_message>(5);
    message->set_field(0, static_cast<int>(message_type::YIELD));
    message->set_field(1, m_request_id);
    message->set_field(2, std::map<int, int>() /* No details */);
    message->set_field(3, arguments);
    message->set_field(4, kw_arguments);

    m_send_result_fn(message);
    m_send_result_fn = send_result_fn();
}

inline void wamp_invocation_impl::error(const std::string& error_uri)
{
    throw_if_not_sendable();

    // [ERROR, INVOCATION, INVOCATION.Request|id, Details|dict, Error|uri]
    auto message = std::make_shared<wamp_message>(5);
    message->set_field(0, static_cast<int>(message_type::ERROR));
    message->set_field(1, static_cast<int>(message_type::INVOCATION));
    message->set_field(2, m_request_id);
    message->set_field(3, std::map<int, int>() /* No details */);
    message->set_field(4, error_uri);

    m_send_result_fn(message);
    m_send_result_fn = send_result_fn();
}

template <typename List>
inline void wamp_invocation_impl::error(const std::string& error_uri, const List& arguments)
{
    throw_if_not_sendable();

    // [ERROR, INVOCATION, INVOCATION.Request|id, Details|dict, Error|uri, Arguments|list]
    auto message = std::make_shared<wamp_message>(6);
    message->set_field(0, static_cast<int>(message_type::ERROR));
    message->set_field(1, static_cast<int>(message_type::INVOCATION));
    message->set_field(2, m_request_id);
    message->set_field(3, std::map<int, int>() /* No details */);
    message->set_field(4, error_uri);
    message->set_field(5, arguments);

    m_send_result_fn(message);
    m_send_result_fn = send_result_fn();
}

template <typename List, typename Map>
inline void wamp_invocation_impl::error(
        const std::string& error_uri,
        const List& arguments, const Map& kw_arguments)
{
    throw_if_not_sendable();

    // [ERROR, INVOCATION, INVOCATION.Request|id, Details|dict, Error|uri, Arguments|list, ArgumentsKw|dict]
    auto message = std::make_shared<wamp_message>(7);
    message->set_field(0, static_cast<int>(message_type::ERROR));
    message->set_field(1, static_cast<int>(message_type::INVOCATION));
    message->set_field(2, m_request_id);
    message->set_field(3, std::map<int, int>() /* No details */);
    message->set_field(4, error_uri);
    message->set_field(5, arguments);
    message->set_field(6, kw_arguments);

    m_send_result_fn(message);
    m_send_result_fn = send_result_fn();
}

inline void wamp_invocation_impl::set_send_result_fn(send_result_fn&& send_result)
{
    m_send_result_fn = std::move(send_result);
}

inline void wamp_invocation_impl::set_request_id(std::uint64_t request_id)
{
    m_request_id = request_id;
}

inline void wamp_invocation_impl::set_zone(msgpack::zone&& zone)
{
    m_zone = std::move(zone);
}

inline void wamp_invocation_impl::set_arguments(const msgpack::object& arguments)
{
    m_arguments = arguments;
}

inline void wamp_invocation_impl::set_kw_arguments(const msgpack::object& kw_arguments)
{
    m_kw_arguments = kw_arguments;
}

inline bool wamp_invocation_impl::sendable() const
{
    return static_cast<bool>(m_send_result_fn);
}

inline void wamp_invocation_impl::throw_if_not_sendable()
{
    if (!sendable()) {
        throw std::runtime_error("tried to call result() or error() but wamp_invocation "
                "is not sendable (double call?)");
    }
}

} // namespace autobahn
