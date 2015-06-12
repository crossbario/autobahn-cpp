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

#include "wamp_arguments.hpp"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace autobahn {

inline wamp_call_result::wamp_call_result()
    : m_arguments(EMPTY_ARGUMENTS)
    , m_kw_arguments(EMPTY_KW_ARGUMENTS)
{
}

inline wamp_call_result::wamp_call_result(std::unique_ptr<msgpack::zone>&& zone)
    : m_arguments(EMPTY_ARGUMENTS)
    , m_kw_arguments(EMPTY_KW_ARGUMENTS)
    , m_zone(std::move(zone))
{
}

inline wamp_call_result::wamp_call_result(const wamp_call_result& other)
{
    m_zone.reset(new msgpack::zone());
    m_arguments = msgpack::object(other.m_arguments, m_zone.get());
    m_kw_arguments = msgpack::object(other.m_kw_arguments, m_zone.get());
}

inline wamp_call_result::wamp_call_result(wamp_call_result&& other)
    : m_arguments(other.m_arguments)
    , m_kw_arguments(other.m_kw_arguments)
    , m_zone(std::move(other.m_zone))
{
    other.m_arguments = EMPTY_ARGUMENTS;
    other.m_kw_arguments = EMPTY_KW_ARGUMENTS;
}

inline wamp_call_result& wamp_call_result::operator=(const wamp_call_result& other)
{
    if (this == &other) {
        return *this;
    }

    m_zone.reset(new msgpack::zone());
    m_arguments = msgpack::object(other.m_arguments, m_zone.get());
    m_kw_arguments = msgpack::object(other.m_kw_arguments, m_zone.get());

    return *this;
}

inline wamp_call_result& wamp_call_result::operator=(wamp_call_result&& other)
{
    if (this == &other) {
        return *this;
    }

    m_arguments = other.m_arguments;
    m_kw_arguments = other.m_kw_arguments;
    m_zone = std::move(other.m_zone);

    other.m_arguments = EMPTY_ARGUMENTS;
    other.m_kw_arguments = EMPTY_KW_ARGUMENTS;

    return *this;
}

inline std::size_t wamp_call_result::number_of_arguments() const
{
    return m_arguments.type == msgpack::type::ARRAY ? m_arguments.via.array.size : 0;
}

inline std::size_t wamp_call_result::number_of_kw_arguments() const
{
    return m_kw_arguments.type == msgpack::type::MAP ? m_kw_arguments.via.map.size : 0;
}

template <typename T>
inline T wamp_call_result::argument(std::size_t index) const
{
    if (m_arguments.type != msgpack::type::ARRAY || m_arguments.via.array.size <= index) {
        throw std::out_of_range("no argument at index " + boost::lexical_cast<std::string>(index));
    }
    return m_arguments.via.array.ptr[index].as<T>();
}

template <typename List>
inline List wamp_call_result::arguments() const
{
    return m_arguments.as<List>();
}

template <typename List>
inline void wamp_call_result::get_arguments(List& args) const
{
    m_arguments.convert(args);
}

template <typename... T>
inline void wamp_call_result::get_each_argument(T&... args) const
{
    auto args_tuple = std::make_tuple(std::ref(args)...);
    m_arguments.convert(args_tuple);
}

template <typename T>
inline T wamp_call_result::kw_argument(const std::string& key) const
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
inline T wamp_call_result::kw_argument(const char* key) const
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
inline T wamp_call_result::kw_argument_or(const std::string& key, const T& fallback) const
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
inline T wamp_call_result::kw_argument_or(const char* key, const T& fallback) const
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
inline Map wamp_call_result::kw_arguments() const
{
    return m_kw_arguments.as<Map>();
}

template <typename Map>
inline void wamp_call_result::get_kw_arguments(Map& kw_args) const
{
    m_kw_arguments.convert(kw_args);
}

inline void wamp_call_result::set_arguments(const msgpack::object& arguments)
{
    m_arguments = arguments;
}

inline void wamp_call_result::set_kw_arguments(const msgpack::object& kw_arguments)
{
    m_kw_arguments = kw_arguments;
}

} // namespace autobahn
