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

#ifndef AUTOBAHN_WAMP_CALL_OPTIONS_HPP
#define AUTOBAHN_WAMP_CALL_OPTIONS_HPP

#include <chrono>

namespace autobahn {

class wamp_call_options
{
public:
    wamp_call_options();

    wamp_call_options(wamp_call_options&& other) = delete;
    wamp_call_options(const wamp_call_options& other) = delete;
    wamp_call_options& operator=(wamp_call_options&& other) = delete;
    wamp_call_options& operator=(const wamp_call_options& other) = delete;

    const std::chrono::milliseconds& timeout() const;

    void set_timeout(const std::chrono::milliseconds& timeout);

private:
    std::chrono::milliseconds m_timeout;
};

} // namespace autobahn

#include "wamp_call_options.ipp"

#endif // AUTOBAHN_WAMP_CALL_OPTIONS_HPP
