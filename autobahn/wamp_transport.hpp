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

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include <boost/thread/future.hpp>
#include <memory>
#include <string>

namespace autobahn {

class wamp_message;
class wamp_transport_handler;

/*!
 * Provides an abstraction for a transport to be used by the session. A wamp
 * transport is defined as being message based, bidirectional, reliable, and
 * ordered.
 */
class wamp_transport
{
public:
    /*!
     * Attempts to connect the transport.
     *
     * @return A future that will be satisfied when the connect attempt
     *         has been made.
     */
    virtual boost::future<void> connect() = 0;

    /*!
     * Attempts to disconnect the transport.
     *
     * @return A future that will be satisfied when the disconnect attempt
     *         has been made.
     */
    virtual boost::future<void> disconnect() = 0;

    /*!
     * Determines if the transport is connected.
     *
     * @return Whether or not the transport is connected.
     */
    virtual bool is_connected() const = 0;

    /*!
     * Attaches a handler to the transport. Only one handler may
     * be attached at any given time.
     *
     * @param handler The handler to attach to this transport.
     */
    virtual void attach(
            const std::shared_ptr<wamp_transport_handler>& handler) = 0;

    /*!
     * Detaches the handler currently attached to the transport.
     */
    virtual void detach() = 0;

    /*!
     * Determines if the transport has a handler attached.
     *
     * @return Whether or not a handler is attached.
     */
    virtual bool has_handler() const = 0;

    /*!
     * Send the message synchronously over the transport.
     *
     * @param message The message to be sent.
     */
    virtual void send(wamp_message&& message) = 0;

    /*!
     * Default virtual destructor.
     */
    virtual ~wamp_transport() = default;
};

} // namespace autobahn

#endif // AUTOBAHN_WAMP_TRANSPORT_HPP
