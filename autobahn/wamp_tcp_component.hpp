#ifndef AUTOBAHN_WAMP_TCP_COMPONENT_HPP
#define AUTOBAHN_WAMP_TCP_COMPONENT_HPP

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include "wamp_network_component.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread/future.hpp>
#include <memory>
#include <string>

namespace autobahn {

/*!
 * A component that provides a TCP based WAMP session.
 */
class wamp_tcp_component :
        public wamp_network_component<
                boost::asio::ip::tcp::socket,
                boost::asio::ip::tcp::endpoint>
{
public:
    wamp_tcp_component(
            boost::asio::io_service& io_service,
            const boost::asio::ip::tcp::endpoint& remote_endpoint,
            const std::string& realm,
            bool debug = false);

    virtual ~wamp_tcp_component() override;

    virtual boost::future<void> start() override;
};

} // namespace autobahn

#include "wamp_tcp_component.ipp"

#endif // AUTOBAHN_WAMP_TCP_COMPONENT_HPP
