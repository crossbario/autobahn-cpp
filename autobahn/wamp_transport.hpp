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

#ifndef AUTOBAHN_WAMP_TRANSPORT_HPP
#define AUTOBAHN_WAMP_TRANSPORT_HPP

#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <functional>

namespace autobahn {

/*!
 * Provides an abstraction for a transport to be used by the session.
 */
class wamp_transport : public boost::noncopyable
{
public:
    /*!
     * Definition of a handler that is to be invoked upon completion
     * of an asynchronous read request.
     *
     * @param error_code Indicates whether or not the read was successful.
     * @param bytes_transferred If the read was successful this indicates
     *        how many bytes were successfully read.
     */
    using read_handler =
            std::function<void(
                    const boost::system::error_code& error_code,
                    std::size_t bytes_transferred)>;

    /*!
     * Definition of a handler that is to be invoked upon completion
     * of an asynchronous connection attempt.
     *
     * @param error_code Indicates whether or not the connect was successful.
     */
    using connect_handler =
            std::function<void(const boost::system::error_code&)>;

public:
    virtual ~wamp_transport();

    /*!
     * Attempts to connect the transport. Should throw an exception if there
     * is a failure.
     */
    virtual void connect() = 0;

    /*!
     * Attempts to connect the transport asynchronously. Should throw if there
     * is an error initiating the connect. Otherwise, any errors in processing
     * the asynchronous connection attempt should be passed to the handler.
     *
     * @param handler The handler to be invoked after the connection attempt
     *        has been made.
     */
    virtual void async_connect(const connect_handler& handler) = 0;

    /*!
     * Attempts to disconnect the transport. Should throw an exception if there
     * is a failure.
     */
    virtual void disconnect() = 0;

    /*!
     * Requests an asynchronous read on the transport. Should throw if there is
     * an error initiating the read. Otherwise, any errors in processing the
     * asynchronous read request should be passed to the handler.
     *
     * @param buffer The serialized buffer to be read.
     * @param length The length of the serialized buffer to be read.
     * @param handler The handler invoked upon completion of the read operation.
     */
    virtual void async_read(
            char* buffer,
            std::size_t length,
            const read_handler& handler) = 0;

    /*!
     * Requests an synchronous write on the transport. Should throw if there is
     * an error fullfillng the write request.
     *
     * @param buffer The serialized buffer to be written.
     * @param length The length of the serialized buffer to be written.
     *
     * @return The actual number of bytes written.
     */
    virtual std::size_t write(
            const char* buffer,
            std::size_t length) = 0;
};

} // namespace autobahn

#include "wamp_transport.ipp"

#endif // AUTOBAHN_WAMP_TRANSPORT_HPP
