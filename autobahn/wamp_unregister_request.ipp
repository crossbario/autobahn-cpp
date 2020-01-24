///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Crossbar.io Technologies GmbH and contributors
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

inline wamp_unregister_request::wamp_unregister_request(const wamp_registration& registration)
    : m_registration(registration)
    , m_response()
{
}

inline wamp_unregister_request::wamp_unregister_request(const wamp_registration& registration,
                                                        on_success_handler&& on_success,
                                                        on_exception_handler&& on_exception)
    : m_registration(registration)
    , m_response(std::move(on_success), std::move(on_exception))
{
}

inline wamp_async<void>& wamp_unregister_request::response()
{
    return m_response;
}

inline void wamp_unregister_request::set_response()
{
    m_response.set_value();
}

inline wamp_registration& wamp_unregister_request::registration()
{
    return m_registration;
}

} // namespace autobahn
