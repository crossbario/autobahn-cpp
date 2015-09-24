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

#include <stdexcept>

namespace autobahn {

inline wamp_message::wamp_message(std::size_t num_fields)
    : m_zone()
    , m_fields(num_fields)
{
}

inline wamp_message::wamp_message(std::size_t num_fields, msgpack::zone&& zone)
    : m_zone(std::move(zone))
    , m_fields(num_fields)
{
}

inline wamp_message::wamp_message(message_fields&& fields, msgpack::zone&& zone)
    : m_zone(std::move(zone))
    , m_fields(std::move(fields))
{
}

inline wamp_message::wamp_message(wamp_message&& other)
{
    m_zone = std::move(other.m_zone);
    m_fields = std::move(other.m_fields);
}

inline wamp_message& wamp_message::operator=(wamp_message&& other)
{
    if (this == &other) {
        return *this;
    }

    m_zone = std::move(other.m_zone);
    m_fields = std::move(other.m_fields);

    return *this;
}

inline const msgpack::object& wamp_message::field(std::size_t index) const
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    return m_fields[index];
}

template <typename Type>
inline Type wamp_message::field(std::size_t index)
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    return m_fields[index].as<Type>();
}

template <typename Type>
inline void wamp_message::set_field(std::size_t index, const Type& type)
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    m_fields[index] = msgpack::object(type, m_zone);
}

inline bool wamp_message::is_field_type(std::size_t index, msgpack::type::object_type type) const
{
    if (index >= m_fields.size()) {
        throw std::out_of_range("invalid message field index");
    }

    return m_fields[index].type == type;
}

inline std::size_t wamp_message::size() const
{
    return m_fields.size();
}

inline wamp_message::message_fields&& wamp_message::fields()
{
    return std::move(m_fields);
}

inline msgpack::zone&& wamp_message::zone()
{
    return std::move(m_zone);
}

} // namespace autobahn
