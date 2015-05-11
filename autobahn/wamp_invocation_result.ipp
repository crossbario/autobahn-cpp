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

namespace autobahn {

inline wamp_invocation_result::wamp_invocation_result()
    : m_zone()
    , m_arguments()
    , m_kw_arguments()
{
}

inline const msgpack::object& wamp_invocation_result::arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_invocation_result::kw_arguments() const
{
    return m_kw_arguments;
}

template <typename ARGUMENTS>
inline void wamp_invocation_result::set_arguments(const ARGUMENTS& arguments)
{
    m_arguments = msgpack::object(arguments, m_zone);
    if (m_arguments.type != msgpack::type::ARRAY) {
        m_arguments = msgpack::object();
        throw std::invalid_argument("response arguments must be an array/vector");
    }
}

template <typename KW_ARGUMENTS>
inline void wamp_invocation_result::set_kw_arguments(const KW_ARGUMENTS& kw_arguments)
{
    m_arguments = msgpack::object(kw_arguments, m_zone);
    if (m_arguments.type != msgpack::type::ARRAY) {
        m_kw_arguments = msgpack::object();
        throw std::invalid_argument("response keyword arguments must be a map");
    }
}

} // namespace autobahn
