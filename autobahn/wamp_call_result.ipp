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

inline const msgpack::object& wamp_call_result::arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_call_result::kw_arguments() const
{
    return m_kw_arguments;
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
