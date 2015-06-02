///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2014 Tavendo GmbH
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AUTOBAHN_WAMP_INVOCATION_HPP
#define AUTOBAHN_WAMP_INVOCATION_HPP

#include "wamp_arguments.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <msgpack.hpp>

namespace autobahn {

class wamp_invocation_impl
{
public:
    wamp_invocation_impl();
    wamp_invocation_impl(wamp_invocation_impl&&) = delete; // copy wamp_invocation instead

    /*!
     * The positional arguments passed to the invocation.
     */
    const msgpack::object& arguments() const;

    /*!
     * The keyword arguments passed to the invocation.
     */
    const msgpack::object& kw_arguments() const;

    /*!
     * Reply to the invocation with an empty result.
     */
    void empty_result();

    /*!
     * Reply to the invocation with positional arguments.
     */
    template <typename List>
    void result(const List& arguments);

    /*!
     * Reply to the invocation with positional and keyword arguments.
     */
    template <typename List, typename Map>
    void result(const List& arguments, const Map& kw_arguments);

    /*!
     * Reply to the invocation with an error and no further details.
     */
    void error(const std::string& error_uri);

    /*!
     * Reply to the invocation with an error, including positional arguments.
     */
    template <typename List>
    void error(const std::string& error_uri, const List& arguments);

    /*!
     * Reply to the invocation with an error, including positional and keyword arguments.
     */
    template <typename List, typename Map>
    void error(
            const std::string& error_uri,
            const List& arguments, const Map& kw_arguments);

    //
    // functions only called internally by wamp_session

    using send_result_fn = std::function<void(const std::shared_ptr<msgpack::sbuffer>&)>;
    void set_send_result_fn(send_result_fn&&);
    void set_request_id(std::uint64_t);
    void set_zone(msgpack::zone&&);
    void set_arguments(const msgpack::object& arguments);
    void set_kw_arguments(const msgpack::object& kw_arguments);
    bool sendable() const;

private:
    void throw_if_not_sendable();

private:
    msgpack::zone m_zone;
    msgpack::object m_arguments;
    msgpack::object m_kw_arguments;
    send_result_fn m_send_result_fn;
    std::uint64_t m_request_id;
};

using wamp_invocation = std::shared_ptr<wamp_invocation_impl>;

} // namespace autobahn

#include "wamp_invocation.ipp"

#endif // AUTOBAHN_WAMP_INVOCATION_HPP
