namespace autobahn {

inline wamp_tcp_component::wamp_tcp_component(
        boost::asio::io_service& io_service,
        const boost::asio::ip::tcp::endpoint& remote_endpoint,
        const std::string& realm,
        bool debug)
    : wamp_network_component(io_service, remote_endpoint, realm, debug)
{
}

inline wamp_tcp_component::~wamp_tcp_component()
{
}

inline boost::future<void> wamp_tcp_component::start()
{
    boost::future<void> started = wamp_network_component::start().then(
        [&](boost::future<void>) {
            // Disable naggle algorithm. Refer to the following for reference:
            //     http://www.techrepublic.com/article/tcp-ip-options-for-high-performance-data-transmission/
            boost::asio::ip::tcp::no_delay option(true);
            socket().set_option(option);
    });

    return started;
}

} // namespace autobahn
