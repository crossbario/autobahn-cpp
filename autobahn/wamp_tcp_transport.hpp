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

#ifndef AUTOBAHN_WAMP_TCP_TRANSPORT_HPP
#define AUTOBAHN_WAMP_TCP_TRANSPORT_HPP

#include "wamp_network_transport.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace autobahn {

/*!
 * A transport that provides support for TCP.
 */
class wamp_tcp_transport :
        public wamp_network_transport<
            boost::asio::ip::tcp::socket,
            boost::asio::ip::tcp::endpoint>
{
public:
    wamp_tcp_transport(
            boost::asio::io_service& io_service,
            const boost::asio::ip::tcp::endpoint& remote_endpoint);
    virtual ~wamp_tcp_transport() override;

    virtual void connect() override;
    virtual void async_connect(
            const wamp_transport::connect_handler& handler) override;
};

} // namespace autobahn

#include "wamp_tcp_transport.ipp"

#endif // AUTOBAHN_WAMP_TCP_TRANSPORT_HPP
