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

#ifndef AUTOBAHN_WAMP_NETWORK_TRANSPORT_HPP
#define AUTOBAHN_WAMP_NETWORK_TRANSPORT_HPP

#include <boost/asio/io_service.hpp>
#include <cstddef>

namespace autobahn {

/*!
 * A helper class for deriving specific network based transports.
 *
 * @tparam Socket The socket type for the transport.
 * @tparam Endpoint The endpoint type for the socket to connect to.
 */
template <typename Socket, typename Endpoint>
class wamp_network_transport : public wamp_transport
{
public:
    typedef Socket socket_type;
    typedef Endpoint endpoint_type;

public:
    /*!
     * Constructs a network transport.
     *
     * @param io_service The io service to use for asynchronous operations.
     * @param remote_endpoint The remote endpoint to connect to.
     */
    wamp_network_transport(
            boost::asio::io_service& io_service,
            const Endpoint& remote_endpoint);

    virtual ~wamp_network_transport() override;

    /*!
     * @copydoc wamp_transport::connect()
     */
    virtual void connect() override;

    /*!
     * @copydoc wamp_transport::async_connect()
     */
    virtual void async_connect(
            const wamp_transport::connect_handler& handler) override;

    /*!
     * @copydoc wamp_transport::disconnect()
     */
    virtual void disconnect() override;


    /*!
     * @copydoc wamp_transport::async_read()
     */
    virtual void async_read(
            char* buffer,
            std::size_t length,
            const wamp_transport::read_handler& handler) override;

    /*!
     * @copydoc wamp_transport::write()
     */
    virtual std::size_t write(
            const char* buffer,
            std::size_t length) override;

protected:
    Socket& socket();

private:
    /*!
     * The underlying socket for the transport.
     */
    Socket m_socket;

    /*!
     * The remote endpoint to connect the socket to.
     */
    Endpoint m_remote_endpoint;
};

} // namespace autobahn

#include "wamp_network_transport.ipp"

#endif // AUTOBAHN_WAMP_NETWORK_TRANSPORT_HPP
