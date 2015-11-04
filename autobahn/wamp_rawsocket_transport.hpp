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

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include "wamp_transport.hpp"

#include <boost/thread/future.hpp>
#include <boost/asio/io_service.hpp>
#include <cstddef>
#include <memory>
#include <msgpack.hpp>

namespace autobahn {

class wamp_message;
class wamp_transport_handler;

/*!
 * A class that represents a rawsocket transport. It is templated based
 * on the socket type
 *
 * @tparam Socket The socket type for the transport.
 * @tparam Endpoint The endpoint type for the socket to connect to.
 */
template <typename Socket>
class wamp_rawsocket_transport :
        public wamp_transport,
        public std::enable_shared_from_this<wamp_rawsocket_transport<Socket>>
{
public:
    /*!
     * Convenience type for the socket being used.
     */
    typedef Socket socket_type;

    /*!
     * Convenience type for the endpoint being used.
     */
    typedef typename Socket::endpoint_type endpoint_type;

public:
    /*!
     * Constructs a rawsocket transport.
     *
     * @param io_service The io service to use for asynchronous operations.
     * @param remote_endpoint The remote endpoint to connect to.
     */
    wamp_rawsocket_transport(
            boost::asio::io_service& io_service,
            const endpoint_type& remote_endpoint,
            bool debug_enabled=false);

    virtual ~wamp_rawsocket_transport() override = default;

    /*!
     * @copydoc wamp_transport::connect()
     */
    virtual boost::future<void> connect() override;

    /*!
     * @copydoc wamp_transport::disconnect()
     */
    virtual boost::future<void> disconnect() override;

    /*!
     * @copydoc wamp_transport::is_connected()
     */
    virtual bool is_connected() const override;

    /*!
     * @copydoc wamp_transport::attach()
     */
    virtual void attach(
            const std::shared_ptr<wamp_transport_handler>& handler) override;

    /*!
     * @copydoc wamp_transport::detach()
     */
    virtual void detach() override;

    /*!
     * @copydoc wamp_transport::has_handler()
     */
    virtual bool has_handler() const override;

    /*!
     * @copydoc wamp_transport::send()
     */
    virtual void send(wamp_message&& message) override;

protected:
    socket_type& socket();

private:

    void handshake_reply_handler(
            const boost::system::error_code& error_code,
            std::size_t /* bytes_transferred */);

    void receive_message();

    void receive_message_header(
            const boost::system::error_code& error,
            std::size_t /* bytes transferred */);

    void receive_message_body(
            const boost::system::error_code& error,
            std::size_t /* bytes transferred */);

private:
    /*!
     * The underlying socket for the transport.
     */
    socket_type m_socket;

    /*!
     * The remote endpoint to connect the socket to.
     */
    endpoint_type m_remote_endpoint;

    /*!
     * The promise that is fulfilled when the connect attempt is complete.
     */
    boost::promise<void> m_connect;

    /*!
     * The promise that is fulfilled when the disconnect attempt is complete.
     */
    boost::promise<void> m_disconnect;

    /*!
     * The handler to be notify.
     */
    std::shared_ptr<wamp_transport_handler> m_handler;

    /*!
     * Buffer used for sending and receiving the handshake.
     */
    uint8_t m_handshake_buffer[4];

    /*!
     * Stores the length of the next serialized message to receive.
     */
    uint32_t m_message_length;

    /*!
     * Used for unpacking serialized messages.
     */
    msgpack::unpacker m_message_unpacker;

    /*!
     * Whether or not debugging is enabled.
     */
    bool m_debug_enabled;
};

} // namespace autobahn

#include "wamp_rawsocket_transport.ipp"

#endif // AUTOBAHN_WAMP_NETWORK_TRANSPORT_HPP
