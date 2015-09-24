#include <cassert>

namespace autobahn {

wamp_component::wamp_component(
            boost::asio::io_service& io_service,
            const std::string& realm,
            bool debug_enabled)
    : m_io_service(io_service)
    , m_realm(realm)
    , m_debug_enabled(debug_enabled)
    , m_session()
{
}

wamp_component::~wamp_component()
{
}

boost::future<void> wamp_component::start()
{
    assert(m_session);

    boost::future<void> started = m_session->start().then(
        boost::launch::any, [&](boost::future<bool>) {
            m_session->join(m_realm).wait();
    });

    return started;
}

boost::future<void> wamp_component::stop()
{
    assert(m_session);

    boost::future<void> stopped = m_session->leave().then(
        boost::launch::any, [&](boost::future<std::string>) {
        m_session->stop().wait();
    });

    return stopped;
}

boost::asio::io_service& wamp_component::io_service()
{
    return m_io_service;
}

inline const std::string& wamp_component::realm() const
{
    return m_realm;
}

bool wamp_component::is_debug_enabled() const
{
    return m_debug_enabled;
}

const std::shared_ptr<autobahn::wamp_session>&
wamp_component::session()
{
    return m_session;
}

void wamp_component::initialize(
        const std::shared_ptr<wamp_transport>& input,
        const std::shared_ptr<wamp_transport>& output)
{
    assert(!m_session);
    m_session = std::make_shared<wamp_session>(
            m_io_service, input, output, m_debug_enabled);
}

} // namespace autobahn
