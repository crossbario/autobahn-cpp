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

#ifndef AUTOBAHN_WAMP_UDS_TRANSPORT_HPP
#define AUTOBAHN_WAMP_UDS_TRANSPORT_HPP

#include "wamp_rawsocket_transport.hpp"

#include <boost/asio/local/stream_protocol.hpp>

namespace autobahn {

/*!
 * A transport that provides rawsocket support over unix domain sockets (UDS).
 */
using wamp_uds_transport =
        wamp_rawsocket_transport<
            boost::asio::local::stream_protocol::socket>;

} // namespace autobahn

#endif // AUTOBAHN_WAMP_UDS_TRANSPORT_HPP
