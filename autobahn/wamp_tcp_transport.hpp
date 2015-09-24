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

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include "wamp_rawsocket_transport.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace autobahn {

/*!
 * A transport that provides rawsocket support over TCP.
 */
class wamp_tcp_transport :
        public wamp_rawsocket_transport<boost::asio::ip::tcp::socket>
{
public:
    wamp_tcp_transport(
            boost::asio::io_service& io_service,
            const boost::asio::ip::tcp::endpoint& remote_endpoint,
            bool debug_enabled=false);
    virtual ~wamp_tcp_transport() override;

    virtual boost::future<void> connect() override;
};

} // namespace autobahn

#include "wamp_tcp_transport.ipp"

#endif // AUTOBAHN_WAMP_TCP_TRANSPORT_HPP
