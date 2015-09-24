#ifndef AUTOBAHN_WAMP_UDS_COMPONENT_HPP
#define AUTOBAHN_WAMP_UDS_COMPONENT_HPP

#include "wamp_network_component.hpp"
#include "wamp_uds_transport.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <memory>
#include <string>

namespace autobahn {

/*!
 * A component that provides a unix doamin socket (UDS) based WAMP session.
 */
using wamp_uds_component = wamp_network_component<wamp_uds_transport>;

} // namespace autobahn

#endif // AUTOBAHN_WAMP_UDS_COMPONENT_HPP
