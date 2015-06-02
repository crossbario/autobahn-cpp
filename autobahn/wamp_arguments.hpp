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

#ifndef AUTOBAHN_WAMP_ARGUMENTS_HPP
#define AUTOBAHN_WAMP_ARGUMENTS_HPP

#include <array>
#include <msgpack.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace autobahn {

using wamp_arguments = std::vector<msgpack::object>;
using wamp_kw_arguments = std::unordered_map<std::string, msgpack::object>;

static const msgpack::object EMPTY_ARGUMENTS(std::array<msgpack::object, 0>(), nullptr);
static const msgpack::object EMPTY_KW_ARGUMENTS(wamp_kw_arguments(), nullptr);

} // namespace autobahn

#endif // AUTOBAHN_WAMP_ARGUMENTS_HPP
