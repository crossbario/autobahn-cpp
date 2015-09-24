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

#include "exceptions.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

namespace autobahn {

template <class Socket, class Endpoint>
wamp_network_transport<Socket, Endpoint>::wamp_network_transport(
            boost::asio::io_service& io_service,
            const Endpoint& remote_endpoint)
    : wamp_transport()
    , m_socket(io_service)
    , m_remote_endpoint(remote_endpoint)
{
}

template <class Socket, class Endpoint>
wamp_network_transport<Socket, Endpoint>::~wamp_network_transport()
{
}

template <class Socket, class Endpoint>
void wamp_network_transport<Socket, Endpoint>::connect()
{
    if (m_socket.is_open()) {
        throw network_error("network transport already connected");
    }

    m_socket.connect(m_remote_endpoint);
}

template <class Socket, class Endpoint>
void wamp_network_transport<Socket, Endpoint>::async_connect(
        const wamp_transport::connect_handler& handler)
{
    if (m_socket.is_open()) {
        throw network_error("network transport already connected");
    }

    m_socket.async_connect(m_remote_endpoint, handler);
}

template <class Socket, class Endpoint>
void wamp_network_transport<Socket, Endpoint>::disconnect()
{
    if (!m_socket.is_open()) {
        throw network_error("network transport already disconnected");
    }

    m_socket.close();
}

template <class Socket, class Endpoint>
void wamp_network_transport<Socket, Endpoint>::async_read(
        char* buffer,
        std::size_t length,
        const wamp_transport::read_handler& handler)
{
    boost::asio::async_read(
            m_socket, boost::asio::buffer(buffer, length), handler);
}

template <class Socket, class Endpoint>
std::size_t wamp_network_transport<Socket, Endpoint>::write(
        const char* buffer,
        std::size_t length)
{
    return boost::asio::write(
            m_socket, boost::asio::buffer(buffer, length));
}


template <class Socket, class Endpoint>
Socket& wamp_network_transport<Socket, Endpoint>::socket()
{
    return m_socket;
}

} // namespace autobahn
