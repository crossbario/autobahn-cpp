#ifndef AUTOBAHN_WAMP_TCP_COMPONENT_HPP
#define AUTOBAHN_WAMP_TCP_COMPONENT_HPP

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include "wamp_network_component.hpp"
#include "wamp_tcp_transport.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/future.hpp>
#include <memory>
#include <string>

namespace autobahn {

/*!
 * A component that provides a TCP based WAMP session.
 */
using wamp_tcp_component = wamp_network_component<wamp_tcp_transport>;

} // namespace autobahn

#endif // AUTOBAHN_WAMP_TCP_COMPONENT_HPP
