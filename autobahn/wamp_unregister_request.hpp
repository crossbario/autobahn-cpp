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

#ifndef AUTOBAHN_WAMP_UNREGISTER_REQUEST_HPP
#define AUTOBAHN_WAMP_UNREGISTER_REQUEST_HPP

#include "boost_config.hpp"
#include "wamp_async.hpp"
#include "wamp_registration.hpp"

namespace autobahn {

/// An outstanding wamp call.
class wamp_unregister_request
{
public:
    using on_success_handler = wamp_async<void>::on_success_handler;
    using on_exception_handler = wamp_async<void>::on_exception_handler;

    wamp_unregister_request(const wamp_registration& registration);
    wamp_unregister_request(const wamp_registration& registration,
                            on_success_handler&& on_success,
                            on_exception_handler&& on_exception);

    wamp_async<void>& response();
    void set_response();
    wamp_registration& registration();

private:
    wamp_registration m_registration;
    wamp_async<void> m_response;
};

} // namespace autobahn

#include "wamp_unregister_request.ipp"

#endif // AUTOBAHN_WAMP_UNREGISTER_REQUEST_HPP
