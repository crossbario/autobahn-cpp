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
        const boost::asio::ip::tcp::endpoint& remote_endpoint)
    : wamp_network_transport(io_service, remote_endpoint)
{
}

inline wamp_tcp_transport::~wamp_tcp_transport()
{
}

inline void wamp_tcp_transport::connect()
{
    wamp_network_transport<
            boost::asio::ip::tcp::socket,
            boost::asio::ip::tcp::endpoint>::connect();

    // Disable naggle for improved performance.
    boost::asio::ip::tcp::no_delay option(true);
    socket().set_option(option);
}

inline void wamp_tcp_transport::async_connect(
        const wamp_transport::connect_handler& handler)
{
    auto wrapped_handler = [=](
            const boost::system::error_code& error_code) {
        if (!error_code) {
            // Disable naggle for improved performance.
            boost::asio::ip::tcp::no_delay option(true);
            socket().set_option(option);
        }

        handler(error_code);
    };

    wamp_network_transport<
            boost::asio::ip::tcp::socket,
            boost::asio::ip::tcp::endpoint>::async_connect(wrapped_handler);
}

} // namespace autobahn
