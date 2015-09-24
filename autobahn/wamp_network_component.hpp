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

template <typename Transport>
class wamp_network_component : public wamp_component
{
public:
    wamp_network_component(
            boost::asio::io_service& io_service,
            const typename Transport::endpoint_type& remote_endpoint,
            const std::string& realm,
            bool debug=false);

    virtual ~wamp_network_component() override;

    virtual boost::future<void> start() override;
    virtual boost::future<void> stop() override;

private:
    std::shared_ptr<Transport> m_transport;
};

} // namespace autobahn

#include "wamp_network_component.ipp"

#endif // AUTOBAHN_WAMP_NETWORK_COMPONENT_HPP
