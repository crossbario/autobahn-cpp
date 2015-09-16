#include "exceptions.hpp"

namespace autobahn {

template <class Socket, class Endpoint>
wamp_network_component<Socket, Endpoint>::wamp_network_component(
            boost::asio::io_service& io_service,
            const Endpoint& remote_endpoint,
            const std::string& realm,
            bool debug)
    : wamp_component<Socket, Socket>(io_service, realm, debug)
    , m_socket(io_service)
    , m_remote_endpoint(remote_endpoint)
{
}

template <class Socket, class Endpoint>
wamp_network_component<Socket, Endpoint>::~wamp_network_component()
{
}

template <class Socket, class Endpoint>
boost::future<void> wamp_network_component<Socket, Endpoint>::start()
{
    if (m_socket.is_open()) {
        throw protocol_error("component already started");
    }

    // Consider async connect and use a promise to chain a
    // future to that returned by the call to start.
    m_socket.connect(m_remote_endpoint);
    wamp_component<Socket, Socket>::initialize(m_socket, m_socket);
    return wamp_component<Socket, Socket>::start();
}

template <class Socket, class Endpoint>
boost::future<void> wamp_network_component<Socket, Endpoint>::stop()
{
    if (!m_socket.is_open()) {
        throw protocol_error("component already stopped");
    }

    boost::future<void> stopped = wamp_component<Socket, Socket>::stop().then(
        [&](boost::future<void>) {
        m_socket.close();
    });

    return stopped;
}

template <class Socket, class Endpoint>
Endpoint wamp_network_component<Socket, Endpoint>::local_endpoint() const
{
    return m_socket.local_endpoint();
}

template <class Socket, class Endpoint>
Endpoint wamp_network_component<Socket, Endpoint>::remote_endpoint() const
{
    return m_remote_endpoint;
}

template <class Socket, class Endpoint>
Socket& wamp_network_component<Socket, Endpoint>::socket()
{
    return m_socket;
}

} // namespace autobahn
