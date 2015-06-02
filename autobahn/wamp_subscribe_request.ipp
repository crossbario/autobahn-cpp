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

inline wamp_subscribe_request::wamp_subscribe_request()
    : m_handler()
    , m_response()
{
}

inline wamp_subscribe_request::wamp_subscribe_request(const wamp_event_handler& handler)
    : m_handler(handler)
    , m_response()
{
}

inline const wamp_event_handler& wamp_subscribe_request::handler() const
{
    return m_handler;
}

inline boost::promise<wamp_subscription>& wamp_subscribe_request::response()
{
    return m_response;
}

inline void wamp_subscribe_request::set_response(const wamp_subscription& subscription)
{
    m_response.set_value(subscription);
}

} // namespace autobahn
