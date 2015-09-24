#include <cassert>

namespace autobahn {

template <class Input, class Output>
wamp_component<Input, Output>::wamp_component(
            boost::asio::io_service& io_service,
            const std::string& realm,
            bool debug_enabled)
    : m_io_service(io_service)
    , m_realm(realm)
    , m_debug_enabled(debug_enabled)
    , m_session()
{
}

template <class Input, class Output>
wamp_component<Input, Output>::~wamp_component()
{
}

template <class Input, class Output>
boost::future<void> wamp_component<Input, Output>::start()
{
    assert(m_session);

    boost::future<void> started = m_session->start().then(
        boost::launch::any, [&](boost::future<bool>) {
            m_session->join(m_realm).wait();
    });

    return started;
}

template <class Input, class Output>
boost::future<void> wamp_component<Input, Output>::stop()
{
    assert(m_session);

    boost::future<void> stopped = m_session->leave().then(
        boost::launch::any, [&](boost::future<std::string>) {
        m_session->stop().wait();
    });

    return stopped;
}

template <class Input, class Output>
boost::asio::io_service& wamp_component<Input, Output>::io_service()
{
    return m_io_service;
}

template <class Input, class Output>
inline const std::string& wamp_component<Input, Output>::realm() const
{
    return m_realm;
}

template <class Input, class Output>
bool wamp_component<Input, Output>::is_debug_enabled() const
{
    return m_debug_enabled;
}

template <class Input, class Output>
const std::shared_ptr<autobahn::wamp_session<Input, Output>>&
wamp_component<Input, Output>::session()
{
    return m_session;
}

template <class Input, class Output>
void wamp_component<Input, Output>::initialize(Input& input, Output& output)
{
    assert(!m_session);
    m_session = std::make_shared<wamp_session<Input, Output>>(
            m_io_service, input, output, m_debug_enabled);
}

} // namespace autobahn
