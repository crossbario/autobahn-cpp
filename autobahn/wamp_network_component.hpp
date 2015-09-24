#ifndef AUTOBAHN_WAMP_NETWORK_COMPONENT_HPP
#define AUTOBAHN_WAMP_NETWORK_COMPONENT_HPP

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include "wamp_component.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/thread/future.hpp>
#include <memory>
#include <string>

namespace autobahn {

template <class Socket, class Endpoint>
class wamp_network_component :
        public wamp_component<Socket, Socket>
{
public:
    wamp_network_component(
            boost::asio::io_service& io_service,
            const Endpoint& remote_endpoint,
            const std::string& realm,
            bool debug=false);

    virtual ~wamp_network_component() override;

    virtual boost::future<void> start() override;
    virtual boost::future<void> stop() override;

    Endpoint local_endpoint() const;
    Endpoint remote_endpoint() const;

protected:
    Socket& socket();

private:
    Socket m_socket;
    Endpoint m_remote_endpoint;
};

} // namespace autobahn

#include "wamp_network_component.ipp"

#endif // AUTOBAHN_WAMP_NETWORK_COMPONENT_HPP
