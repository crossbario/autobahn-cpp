#ifndef AUTOBAHN_WAMP_COMPONENT_HPP
#define AUTOBAHN_WAMP_COMPONENT_HPP

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include "wamp_session.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/thread/future.hpp>
#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

namespace autobahn {

template <class Input, class Output>
class wamp_component : public boost::noncopyable
{
public:
    wamp_component(
            boost::asio::io_service& io_service,
            const std::string& realm,
            bool debug_enabled=false);
    virtual ~wamp_component();

    /*!
     * Starts the underlying session and joins the session to the target realm.
     *
     * @return The future to wait on indicating that the start procedure is complete.
     */
    virtual boost::future<void> start() = 0;

    /*!
     * Initiates the session to leaves the target realm and then stops the session.
     *
     * @return The future to wait on indicating that the stop procedure is complete.
     */
    virtual boost::future<void> stop() = 0;

    /*!
     * @return The io service associated with the session.
     */
    boost::asio::io_service& io_service();

    /*!
     * @return The realm for the session.
     */
    const std::string& realm() const;

    /*!
     * @return Whether or not debugging is enabled.
     */
    bool is_debug_enabled() const;

    /*!
     * @return The underlying session.
     */
    const std::shared_ptr<autobahn::wamp_session<Input, Output>>& session();

protected:
    /*!
     * To be called by the derived class when the input and output streams
     * have been determined. This must be prior to trying to start the
     * component as this actually creates the underlying session object.
     */
    void initialize(Input& input, Output& output);

private:
    boost::asio::io_service& m_io_service;
    const std::string m_realm;
    const bool m_debug_enabled;

    std::shared_ptr<autobahn::wamp_session<Input, Output>> m_session;
};

} // namespace autobahn

#include "wamp_component.ipp"

#endif // AUTOBAHN_WAMP_COMPONENT_HPP
