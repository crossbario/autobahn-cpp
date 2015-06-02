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

inline wamp_unsubscribe_request::wamp_unsubscribe_request()
    : m_response()
{
}

inline boost::promise<void>& wamp_unsubscribe_request::response()
{
    return m_response;
}

inline void wamp_unsubscribe_request::set_response()
{
    m_response.set_value();
}

} // namespace autobahn
