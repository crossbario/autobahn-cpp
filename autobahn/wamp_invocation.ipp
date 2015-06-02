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

inline wamp_invocation::wamp_invocation()
    : m_arguments(EMPTY_ARGUMENTS)
    , m_kw_arguments(EMPTY_KW_ARGUMENTS)
    , m_result()
{
}

inline const msgpack::object& wamp_invocation::arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_invocation::kw_arguments() const
{
    return m_kw_arguments;
}

inline void wamp_invocation::set_arguments(const msgpack::object& arguments)
{
    m_arguments = arguments;
}

inline void wamp_invocation::set_kw_arguments(const msgpack::object& kw_arguments)
{
    m_kw_arguments = kw_arguments;
}

inline wamp_invocation_result& wamp_invocation::result()
{
    return m_result;
}

} // namespace autobahn
