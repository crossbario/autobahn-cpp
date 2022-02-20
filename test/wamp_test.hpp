//
// Created by Alexander Arlt on 08.02.22.
//

#pragma once

#include <autobahn/autobahn.hpp>
#include <autobahn/wamp_websocketpp_websocket_transport.hpp>
#include <functional>
#include <utility>
#include <boost/optional.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <botan/auto_rng.h>
#include <botan/ecdsa.h>
#include <botan/ec_group.h>
#include <botan/pubkey.h>
#include <botan/hex.h>
#include <botan/ed25519.h>

namespace wamp_test
{

struct websocket_transport_base
{
    websocketpp::client<websocketpp::config::asio_client> ws_client;
    boost::asio::io_service io;
    websocket_transport_base() { ws_client.init_asio(&io); }
};

struct websocket_transport: public websocket_transport_base,
                            public autobahn::wamp_websocketpp_websocket_transport<websocketpp::config::asio_client>
{
    typedef websocket_transport Transport;

    websocket_transport(std::string uri, bool debug)
        : autobahn::wamp_websocketpp_websocket_transport<websocketpp::config::asio_client>(ws_client, uri, debug)
    {
    }
};

class Secret
{
public:
    Secret() = default;
    explicit Secret(std::string _secret): m_secret(std::move(_secret)) {}
    const std::string& secret() { return m_secret; }

private:
    std::string m_secret{};
};

class Ticket
{
public:
    Ticket() = default;
    explicit Ticket(std::string _ticket): m_ticket(std::move(_ticket)) {}
    const std::string& ticket() { return m_ticket; }

private:
    std::string m_ticket{};
};

class Cryptosign
{
public:
    explicit Cryptosign(const Botan::secure_vector<uint8_t>& _privateKey): m_private_key(_privateKey) {}

    const std::vector<uint8_t>& publicKey() { return m_private_key.get_public_key(); }

    std::vector<uint8_t> sign(const std::vector<uint8_t>& _data)
    {
        Botan::AutoSeeded_RNG rng;
        Botan::PK_Signer signer(m_private_key, rng, "Pure");
        signer.update(_data);
        return signer.signature(rng);
    }
    const Botan::Ed25519_PrivateKey& privateKey() const { return m_private_key; }

private:
    Botan::Ed25519_PrivateKey m_private_key;
    Botan::secure_vector<uint8_t> m_publickey{};
};

class test_session: public autobahn::wamp_session
{
public:
    virtual const std::vector<std::string>& authmethods() = 0;
    virtual std::map<std::string, std::string> authextras() { return {}; };

protected:
    test_session(boost::asio::io_service& io_service, bool debug_enabled = false)
        : autobahn::wamp_session(io_service, debug_enabled)
    {
    }
};

class cra_session: public test_session
{
public:
    const std::vector<std::string>& authmethods()
    {
        static std::vector<std::string> authmethods = {"wampcra"};
        return authmethods;
    }

    typedef cra_session Session;

    cra_session(boost::asio::io_service& io, bool debug, Secret _secret)
        : test_session(io, debug), m_secret(std::move(_secret))
    {
    }

    boost::promise<autobahn::wamp_authenticate> challenge_future;
    boost::future<autobahn::wamp_authenticate> on_challenge(const autobahn::wamp_challenge& challenge)
    {
        std::string signature = compute_wcs(m_secret.secret(), challenge.challenge());
        challenge_future.set_value(autobahn::wamp_authenticate(signature));
        return challenge_future.get_future();
    }

private:
    Secret m_secret;
};

class cryptosign_session: public test_session
{
public:
    const std::vector<std::string>& authmethods() override
    {
        static std::vector<std::string> authmethods = {"cryptosign"};
        return authmethods;
    }

    std::map<std::string, std::string> authextras() override
    {
        return {{"pubkey", Botan::hex_encode(m_privateKey.publicKey(), false)}};
    };

    typedef cra_session Session;

    cryptosign_session(boost::asio::io_service& io, bool debug, Cryptosign _privateKey)
        : test_session(io, debug), m_privateKey(std::move(_privateKey))
    {
    }

    boost::promise<autobahn::wamp_authenticate> challenge_future;
    boost::future<autobahn::wamp_authenticate> on_challenge(const autobahn::wamp_challenge& challenge) override
    {
        Botan::AutoSeeded_RNG rng;
        Botan::PK_Signer signer(m_privateKey.privateKey(), rng, "Pure");
        signer.update(Botan::hex_decode(challenge.challenge()));
        std::string signature = Botan::hex_encode(signer.signature(rng));
        challenge_future.set_value(autobahn::wamp_authenticate(signature + challenge.challenge()));
        return challenge_future.get_future();
    }

private:
    Cryptosign m_privateKey;
};

class ticket_session: public test_session
{
public:
    const std::vector<std::string>& authmethods() override
    {
        static std::vector<std::string> authmethods = {"ticket"};
        return authmethods;
    }

    typedef ticket_session Session;

    ticket_session(boost::asio::io_service& io, bool debug, Ticket _ticket)
        : test_session(io, debug), m_ticket(std::move(_ticket))
    {
    }

    boost::promise<autobahn::wamp_authenticate> challenge_future;
    boost::future<autobahn::wamp_authenticate> on_challenge(const autobahn::wamp_challenge& challenge) override
    {
        challenge_future.set_value(autobahn::wamp_authenticate(m_ticket.ticket()));
        return challenge_future.get_future();
    }

private:
    Ticket m_ticket;
};

template<typename TConfig, typename TTransport = websocket_transport>
struct fixture: public TConfig
{
    typedef typename TTransport::Transport Transport;
    typedef test_session Session;

    template<typename TAuthenticateWith>
    bool join_realm(
        const std::string& _realm,
        const std::string& _authid,
        const TAuthenticateWith _authenticate_with,
        std::function<void(Transport&, Session&)> _on_success = [](Transport& transport, Session& session) {})
    {
        std::shared_ptr<Transport> transport = std::make_shared<Transport>(TConfig::uri, TConfig::debug);
        return join_realm(*transport, _realm, _authid, _authenticate_with, _on_success);
    }

    template<typename TAuthenticateWith>
    bool join_realm(
        const std::string& _authid,
        const TAuthenticateWith _authenticate_with,
        std::function<void(Transport&, Session&)> _on_success = [](Transport& transport, Session& session) {})
    {
        std::shared_ptr<Transport> transport = std::make_shared<Transport>(TConfig::uri, TConfig::debug);
        return join_realm(*transport, TConfig::realm, _authid, _authenticate_with, _on_success);
    }

    template<typename TResult, typename TArguments>
    boost::optional<TResult> call(const std::string& procedure, TArguments _arguments)
    {
        autobahn::wamp_call_options call_options;
        call_options.set_timeout(std::chrono::seconds(10));
        return m_session->call(procedure, _arguments, call_options)
            .then(
                [&](boost::future<autobahn::wamp_call_result> _result)
                {
                    boost::optional<TResult> result;
                    try
                    {
                        result = _result.get().argument<TResult>(0);
                    }
                    catch (const std::exception& e)
                    {
                        result.reset();
                    }
                    return result;
                })
            .get();
    }

protected:
    Transport* m_transport;
    Session* m_session;

private:
    bool join_realm(
        Transport& _transport,
        const std::string& _realm,
        const std::string& _authid,
        const Ticket& _ticket,
        std::function<void(Transport&, Session&)> _on_success)
    {
        std::shared_ptr<ticket_session> session
            = std::make_shared<ticket_session>(_transport.io, TConfig::debug, _ticket);
        _transport.attach(std::static_pointer_cast<autobahn::wamp_transport_handler>(session));
        return join(_transport, *session, _realm, _authid, _on_success);
    }

    bool join_realm(
        Transport& _transport,
        const std::string& _realm,
        const std::string& _authid,
        const Secret& _secret,
        std::function<void(Transport&, Session&)> _on_success)
    {
        std::shared_ptr<cra_session> session = std::make_shared<cra_session>(_transport.io, TConfig::debug, _secret);
        _transport.attach(std::static_pointer_cast<autobahn::wamp_transport_handler>(session));
        return join(_transport, *session, _realm, _authid, _on_success);
    }

    bool join_realm(
        Transport& _transport,
        const std::string& _realm,
        const std::string& _authid,
        const Cryptosign& _privateKey,
        std::function<void(Transport&, Session&)> _on_success)
    {
        std::shared_ptr<cryptosign_session> session
            = std::make_shared<cryptosign_session>(_transport.io, TConfig::debug, _privateKey);
        _transport.attach(std::static_pointer_cast<autobahn::wamp_transport_handler>(session));
        return join(_transport, *session, _realm, _authid, _on_success);
    }

    bool join(
        Transport& _transport,
        Session& _session,
        const std::string& _realm,
        const std::string& _authid,
        std::function<void(Transport&, Session&)> _on_success)
    {
        m_transport = &_transport;
        m_session = &_session;

        bool result = false;
        // Make sure the continuation futures we use do not run out of scope prematurely.
        // Since we are only using one thread here this can cause the io service to block
        // as a future generated by a continuation will block waiting for its promise to be
        // fulfilled when it goes out of scope. This would prevent the session from receiving
        // responses from the router.
        boost::future<void> connect_future;
        boost::future<void> start_future;
        boost::future<void> join_future;
        boost::future<void> leave_future;
        boost::future<void> stop_future;

        connect_future = _transport.connect().then(
            [&](boost::future<void> connected)
            {
                try
                {
                    connected.get();
                }
                catch (const std::exception& e)
                {
                    if (TConfig::debug)
                        std::cerr << e.what() << std::endl;
                    _transport.io.stop();
                    return;
                }
                if (TConfig::debug)
                    std::cerr << "transport connected" << std::endl;

                result = true;

                start_future = _session.start().then(
                    [&](boost::future<void> started)
                    {
                        try
                        {
                            started.get();
                        }
                        catch (const std::exception& e)
                        {
                            if (TConfig::debug)
                                std::cerr << e.what() << std::endl;
                            _transport.io.stop();
                            result = false;
                            return;
                        }

                        if (TConfig::debug)
                            std::cerr << "session started" << std::endl;

                        join_future = _session.join(_realm, _session.authmethods(), _authid, _session.authextras())
                                          .then(
                                              [&](boost::future<uint64_t> joined)
                                              {
                                                  try
                                                  {
                                                      auto j = joined.get();
                                                      if (TConfig::debug)
                                                          std::cerr << "joined realm: " << j << std::endl;
                                                  }
                                                  catch (const std::exception& e)
                                                  {
                                                      if (TConfig::debug)
                                                          std::cerr << e.what() << std::endl;
                                                      _transport.io.stop();
                                                      result = false;
                                                      return;
                                                  }

                                                  _on_success(_transport, _session);

                                                  leave_future = _session.leave().then(
                                                      [&](boost::future<std::string> reason)
                                                      {
                                                          try
                                                          {
                                                              auto r = reason.get();
                                                              if (TConfig::debug)
                                                                  std::cerr << "left session (" << r << ")"
                                                                            << std::endl;
                                                          }
                                                          catch (const std::exception& e)
                                                          {
                                                              if (TConfig::debug)
                                                                  std::cerr << "failed to leave session: " << e.what()
                                                                            << std::endl;
                                                              _transport.io.stop();
                                                              result = false;
                                                              return;
                                                          }

                                                          stop_future = _session.stop().then(
                                                              [&](boost::future<void> stopped)
                                                              {
                                                                  if (TConfig::debug)
                                                                      std::cerr << "stopped session" << std::endl;
                                                                  _transport.io.stop();
                                                              });
                                                      });
                                              });
                    });
            });

        try
        {
            if (TConfig::debug)
                std::cerr << "starting io service" << std::endl;
            _transport.io.run();
            if (TConfig::debug)
                std::cerr << "stopped io service" << std::endl;

            connect_future.get();
        }
        catch (...)
        {
            result = false;
        }

        m_transport = nullptr;
        m_session = nullptr;

        return result;
    }
};

} // namespace wamp_test
