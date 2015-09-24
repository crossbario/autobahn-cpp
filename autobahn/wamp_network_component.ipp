#include "exceptions.hpp"

namespace autobahn {

template <typename Transport>
wamp_network_component<Transport>::wamp_network_component(
            boost::asio::io_service& io_service,
            const typename Transport::endpoint_type& remote_endpoint,
            const std::string& realm,
            bool debug)
    : wamp_component(io_service, realm, debug)
    , m_transport(std::make_shared<Transport>(io_service, remote_endpoint))
{
}

template <typename Transport>
wamp_network_component<Transport>::~wamp_network_component()
{
}

template <typename Transport>
boost::future<void> wamp_network_component<Transport>::start()
{
    m_transport->connect();
    wamp_component::initialize(m_transport, m_transport);
    return wamp_component::start();
}

template <typename Transport>
boost::future<void> wamp_network_component<Transport>::stop()
{
    boost::future<void> stopped = wamp_component::stop().then(
        [&](boost::future<void>) {
        m_transport->disconnect();
    });

    return stopped;
}

} // namespace autobahn
