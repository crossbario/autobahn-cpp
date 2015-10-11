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
     * Handler to invoke when pausing transport transmission.
     */
    using pause_handler = std::function<void()>;

    /*!
     * Handler to invoke when resuming transport transmission.
     */
    using resume_handler = std::function<void()>;

public:
    /*
     * CONNECTION INTERFACE
     */
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

    /*
     * SENDER INTERFACE
     */
    /*!
     * Send the message synchronously over the transport.
     *
     * @param message The message to be sent.
     */
    virtual void send(const std::shared_ptr<wamp_message>& message) = 0;

    /*!
     * Set the handler to be invoked when the transport detects congestion
     * sending to the remote peer and needs to apply backpressure on the
     * application.
     *
     * @param handler The pause handler to be invoked.
     */
    virtual void set_pause_handler(const pause_handler& handler) = 0;

    /*!
     * Set the handler to be invoked when the transport detects congestion
     * has subsided on the remote peer and the application can resume sending
     * messages.
     *
     * @param handler The resume handler to be invoked.
     */
    virtual void set_resume_handler(const resume_handler& handler) = 0;

    /*
     * RECEIVER INTERFACE
     */
    /*!
     * Pause receiving of messages. This will prevent the transport from receiving
     * any more messages until it has been resumed. This is used to excert
     * backpressure on the sending peer.
     */
    virtual void pause() = 0;

    /*!
     * Resume receiving of messages. The transport will now begin receiving messsages
     * again and lift backpressure from the sending peer.
     */
    virtual void resume() = 0;

    /*!
     * Attaches a handler to the transport. Only one handler may
     * be attached at any given time.
     *
     * @param handler The handler to attach to this transport.
     *
     * @return A future that will be satisfied when the transport
     *         handler has been successfully attached.
     */
    virtual void attach(
            const std::shared_ptr<wamp_transport_handler>& handler) = 0;

    /*!
     * Detaches the handler currently attached to the transport.
     *
     * @return A future that will be satisfied when the transport
     *         handler has been successfully detached.
     */
    virtual void detach() = 0;

    /*!
     * Determines if the transport has a handler attached.
     *
     * @return Whether or not a handler is attached.
     */
    virtual bool has_handler() const = 0;

    /*!
     * Default virtual destructor.
     */
    virtual ~wamp_transport() = default;
};

} // namespace autobahn

#endif // AUTOBAHN_WAMP_TRANSPORT_HPP
