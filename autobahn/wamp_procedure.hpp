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

#ifndef AUTOBAHN_WAMP_PROCEDURE_HPP
#define AUTOBAHN_WAMP_PROCEDURE_HPP

#include "wamp_arguments.hpp"
#include "wamp_invocation.hpp"

namespace autobahn {

/// Handler type for use with wamp_session::provide
typedef std::function<void(wamp_invocation&)> wamp_procedure;

typedef wamp_kw_arguments provide_options;

} // namespace autobahn

#endif // AUTOBAHN_WAMP_PROCEDURE_HPP