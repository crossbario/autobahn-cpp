#ifndef EXAMPLES_PARAMETERS_HPP
#define EXAMPLES_PARAMETERS_HPP

#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <string>

class parameters
{
public:
    parameters();

    bool debug() const;
    const std::string& realm() const;
    const boost::asio::ip::tcp::endpoint& rawsocket_endpoint() const;

    void set_debug(bool enabled);
    void set_realm(const std::string& realm);
    void set_rawsocket_endpoint(const std::string& ip_address, uint16_t port);

private:
    bool m_debug;
    std::string m_realm;
    boost::asio::ip::tcp::endpoint m_rawsocket_endpoint;
};

std::unique_ptr<parameters> get_parameters(int argc, char** argv);

#endif // EXAMPLES_PARAMETERS_HPP
