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

#ifndef AUTOBAHN_WAMP_TRANSPORT_HANDLER_HPP
#define AUTOBAHN_WAMP_TRANSPORT_HANDLER_HPP

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include "wamp_message.hpp"

#include <boost/thread/future.hpp>
#include <memory>
#include <string>

namespace autobahn {

class wamp_transport;

/*!
 * Provides an abstraction for associating a handler with a transport.
 */
class wamp_transport_handler
{
public:
    /*!
     * Called by the transport when attaching a handler.
     *
     * @param transport The transport being attached to.
     */
    virtual void on_attach(const std::shared_ptr<wamp_transport>& transport) = 0;

    /*!
     * Called by the transport when detaching a handler.
     *
     * @param was_clean Whether or not the transport is cleanly detaching.
     * @param reason The reason for detaching.
     */
    virtual void on_detach(bool was_clean, const std::string& reason) = 0;

    /*!
     * Called by the transport when a message is received.
     *
     * @param message The message that has been received.
     */
    virtual void on_message(wamp_message&& message) = 0;

    /*!
     * Default virtual destructor.
     */
    virtual ~wamp_transport_handler() = default;
};

} // namespace autobahn

#endif // AUTOBAHN_WAMP_TRANSPORT_HANDLER_HPP
