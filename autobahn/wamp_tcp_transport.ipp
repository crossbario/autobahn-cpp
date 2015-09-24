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

#include "wamp_tcp_transport.hpp"

#include <boost/system/error_code.hpp>

namespace autobahn {

inline wamp_tcp_transport::wamp_tcp_transport(
        boost::asio::io_service& io_service,
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        bool debug_enabled)
    : wamp_rawsocket_transport<boost::asio::ip::tcp::socket>(
            io_service, remote_endpoint, debug_enabled)
{
}

inline wamp_tcp_transport::~wamp_tcp_transport()
{
}

inline boost::future<void> wamp_tcp_transport::connect()
{
    return wamp_rawsocket_transport<boost::asio::ip::tcp::socket>::connect().then(
        [&](boost::future<void> connected) {
            // Check the originating future for exceptions.
            connected.get();

            // Disable naggle for improved performance.
            boost::asio::ip::tcp::no_delay option(true);
            socket().set_option(option);
        }
    );
}

} // namespace autobahn
