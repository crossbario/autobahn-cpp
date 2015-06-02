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

#ifndef AUTOBAHN_WAMP_INVOCATION_HPP
#define AUTOBAHN_WAMP_INVOCATION_HPP

#include "wamp_arguments.hpp"
#include "wamp_invocation_result.hpp"

#include <msgpack.hpp>

namespace autobahn {

class wamp_invocation
{
public:
    wamp_invocation();

    const msgpack::object& arguments() const;
    const msgpack::object& kw_arguments() const;

    void set_arguments(const msgpack::object& arguments);
    void set_kw_arguments(const msgpack::object& kw_arguments);

    wamp_invocation_result& result();

private:
    msgpack::object m_arguments;
    msgpack::object m_kw_arguments;
    wamp_invocation_result m_result;
};

} // namespace autobahn

#include "wamp_invocation.ipp"

#endif // AUTOBAHN_WAMP_INVOCATION_HPP
