#include "parameters.hpp"

#include <cstdlib>
#include <boost/asio/ip/address.hpp>
#include <boost/program_options.hpp>
#include <iostream>

namespace {
const std::string LOCALHOST_IP_ADDRESS_STRING("127.0.0.1");
const boost::asio::ip::address LOCALHOST_IP_ADDRESS(
        boost::asio::ip::address::from_string(LOCALHOST_IP_ADDRESS_STRING));
const std::string DEFAULT_WAMP_REALM("default");
const uint16_t DEFAULT_WAMP_RAWSOCKET_PORT(8000);
}

parameters::parameters()
    : m_debug(false)
    , m_realm(DEFAULT_WAMP_REALM)
    , m_rawsocket_endpoint(LOCALHOST_IP_ADDRESS, DEFAULT_WAMP_RAWSOCKET_PORT)
{
}

bool parameters::debug() const
{
    return m_debug;
}

const std::string& parameters::realm() const
{
    return m_realm;
}

const boost::asio::ip::tcp::endpoint& parameters::rawsocket_endpoint() const
{
    return m_rawsocket_endpoint;
}

void parameters::set_debug(bool value)
{
    m_debug = value;
}

void parameters::set_realm(const std::string& realm)
{
    m_realm = realm;
}

void parameters::set_rawsocket_endpoint(const std::string& ip_address, uint16_t port)
{
    m_rawsocket_endpoint = boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string(ip_address), port);
}

std::unique_ptr<parameters> get_parameters(int argc, char** argv)
{
    std::unique_ptr<parameters> params(new parameters);

    namespace po = boost::program_options;
    po::options_description description("options");
    description.add_options()
            ("help", "Display this help message")
            ("realm,r", po::value<std::string>()->default_value(DEFAULT_WAMP_REALM),
                    "The realm to join on the wamp router.")
            ("rawsocket-ip,h", po::value<std::string>()->default_value(LOCALHOST_IP_ADDRESS_STRING),
                    "The ip address of the host running the wamp router.")
            ("rawsocket-port,p", po::value<uint16_t>()->default_value(DEFAULT_WAMP_RAWSOCKET_PORT),
                    "The port that the wamp router is listening for connections on.")
            ("debug,d", po::bool_switch()->default_value(false),
                    "Enable debug logging.");

    po::variables_map variables;
    try {
        po::store(po::parse_command_line(argc, argv, description), variables);

        if (variables.count("help")) {
            std::cout << "Example Parameters" << std::endl
                    << description << std::endl;
            exit(0);
        }

        po::notify(variables);
    } catch(po::error& e) {
        std::cerr << "error: " << e.what() << std::endl << std::endl;
        std::cerr << description << std::endl;
        exit(-1);
    }

    params->set_debug(variables["debug"].as<bool>());
    params->set_realm(variables["realm"].as<std::string>());
    params->set_rawsocket_endpoint(
            variables["rawsocket-ip"].as<std::string>(),
            variables["rawsocket-port"].as<uint16_t>());

    return params;
}
