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

#ifndef AUTOBAHN_WAMP_EVENT_HPP
#define AUTOBAHN_WAMP_EVENT_HPP

#include "wamp_arguments.hpp"

#include <memory>
#include <msgpack.hpp>
#include <string>

namespace autobahn {

class wamp_event
{
public:
    wamp_event(msgpack::zone&& zone);

    /*!
     * The number of positional arguments published by the event.
     */
    std::size_t number_of_arguments() const;

    /*!
     * The number of keyword arguments published by the event.
     */
    std::size_t number_of_kw_arguments() const;

    /*!
     * The positional argument published by the event with the given @p index, converted to type T.
     *
     * Example:
     * `std::string id = event.argument<std::string>(0); // first positional argument`
     *
     * @throw std::out_of_range
     * @throw std::bad_cast
     */
    template <typename T>
    T argument(std::size_t index) const;

    /*!
     * The positional arguments published by the event, converted to a list type.
     *
     * Example:
     * `auto args = event.arguments<std::tuple<std::string>>();`
     *
     * @throw std::bad_cast
     */
    template <typename List>
    List arguments() const;

    /*!
     * Convert and assign the positional arguments published by the event to the given @p args list.
     *
     * Example:
     * ```
     * std::tuple<std::string> args;
     * event.get_arguments(args);
     * ```
     *
     * @throw std::bad_cast
     */
    template <typename List>
    void get_arguments(List& args) const;

    /*!
     * Convert and assign the positional arguments to a given list of individual parameters.
     *
     * Enables a syntax that lets you declare variables individually, but list them in a
     * single space to empathize parameter order. This will also throw if the number of
     * arguments to the invocation doesn't match the number of given parameters.
     *
     * Example:
     * ```
     * uint64_t id;
     * std::string name;
     * invocation->get_each_argument(id, name);
     * ```
     *
     * @throw std::bad_cast
     */
    template <typename... T>
    inline void get_each_argument(T&... args) const;

    /*!
     * The keyword argument published by the event with the given @p key, converted to type T.
     *
     * Overloads are provided for `std::string` and `char*` as @p key type.
     *
     * This function uses key string comparisons to find the matching value, O(n) with n being
     * the number of map elements. Memory allocation for keys is avoided though. For larger maps,
     * you might want to prioritize look-up performance by using `std::map`, `std::unordered_map`
     * or custom types with custom deserialization. To do this, use kw_arguments<Map>() or
     * get_kw_arguments<Map>(map), then access the items from there.
     *
     * Example:
     * `std::string id = event.kw_argument<std::string>("id");`
     *
     * @throw std::out_of_range
     * @throw std::bad_cast
     */
    template <typename T>
    T kw_argument(const std::string& key) const;

    template <typename T>
    T kw_argument(const char* key) const;

    /*!
     * The keyword argument published by the event with the given @p key, converted to type T,
     * or the given @p fallback if no such key was passed.
     *
     * Overloads are provided for `std::string` and `char*` as @p key type.
     *
     * This function uses key string comparisons to find the matching value, O(n) with n being
     * the number of map elements. Memory allocation for keys is avoided though. For larger maps,
     * you might want to prioritize look-up performance by using `std::map`, `std::unordered_map`
     * or custom types with custom deserialization. To do this, use kw_arguments<Map>() or
     * get_kw_arguments<Map>(map), then access the items from there.
     *
     * Example:
     * `std::string id = event.kw_argument_or("id", std::string());`
     *
     * @throw std::bad_cast
     */
    template <typename T>
    T kw_argument_or(const std::string& key, const T& fallback) const;

    template <typename T>
    T kw_argument_or(const char* key, const T& fallback) const;

    /*!
     * The keyword arguments published by the event, converted to a map type.
     *
     * Example:
     * `auto kw_args = event.kw_arguments<std::unordered_map<std::string, msgpack::object>>();`
     *
     * @throw std::bad_cast
     */
    template <typename Map>
    Map kw_arguments() const;

    /*!
     * Convert and assign the keyword arguments published by the event to the given @p kw_args map.
     *
     * Example:
     * ```
     * std::unordered_map<std::string, msgpack::object> kw_args;
     * event.get_kw_arguments(kw_args);
     * ```
     *
     * @throw std::bad_cast
     */
    template <typename Map>
    void get_kw_arguments(Map& kw_args) const;

    //
    // functions only called internally by wamp_session

    void set_arguments(const msgpack::object& arguments);
    void set_kw_arguments(const msgpack::object& kw_arguments);

private:
    msgpack::zone m_zone;
    msgpack::object m_arguments;
    msgpack::object m_kw_arguments;
};

} // namespace autobahn

#include "wamp_event.ipp"

#endif // AUTOBAHN_WAMP_EVENT_HPP
