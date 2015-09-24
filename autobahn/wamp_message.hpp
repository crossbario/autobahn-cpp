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

#ifndef AUTOBAHN_WAMP_MESSAGE_HPP
#define AUTOBAHN_WAMP_MESSAGE_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace autobahn {

/*!
 * A class that represents a wamp message in its simplest form.
 *
 *     [field1, field2, field3, ...]
 *
 * Although not all arrays are valid wamp messages, this class
 * simply provides the building blocks to construct any type of
 * message.
 *
 * TODO: Investigate the benefits of creating a heirarchy of
 *       wamp message types similar to what has been done for
 *       bonefish.
 */
class wamp_message
{
public:
    /*!
     * A convenience type for representing message fields
     */
    using message_fields = std::vector<msgpack::object>;

public:
    /*!
     * Constructs a wamp message with the given number of fields.
     *
     * @param num_fields The number of fields in the message.
     */
    wamp_message(std::size_t num_fields);

    /*!
     * Constructs a wamp message with the given number of fields.
     *
     * @param num_fields The number of fields in the message.
     * @param zone The zone used to allocate fields in the message.
     */
    wamp_message(std::size_t num_fields, msgpack::zone&& zone);

    /*!
     * Constructs a wamp message with the given fields.
     *
     * @param fields The fields in the message.
     * @param zone The zone used to allocate fields in the message.
     */
    wamp_message(message_fields&& fields, msgpack::zone&& zone);

    wamp_message(const wamp_message& other) = delete;
    wamp_message(wamp_message&& other);

    wamp_message& operator=(const wamp_message& other) = delete;
    wamp_message& operator=(wamp_message&& other);

    /*!
     * Retrieves the field at the specified index. Throws an exception
     * if the index is out of bounds.
     *
     * @param index The index of the target field.
     *
     * @return The retrieved type.
     */
    const msgpack::object& field(std::size_t index) const;

    /*!
     * Retrieves the field at the specified index. Throws an exception
     * if the index is out of bounds or if the field cannot be retrieved
     * as the specified type.
     *
     * @tparam Type The field's type.
     * @param index The index of the target field.
     *
     * @return The retrieved type.
     */
    template <typename Type>
    Type field(std::size_t index);

    /*!
     * Sets the field at the specified index. Throws an exception if the
     * index is out of bounds.
     *
     * @tparam Type The field's type.
     * @param index The index of the target field.
     * @param type The type to store in the target field.
     */
    template <typename Type>
    void set_field(std::size_t index, const Type& type);

    /*!
     * Determines if the field at the specified index is of the given type.
     *
     * @param index The index of the target field.
     * @param type The field type to check against.
     */
    bool is_field_type(std::size_t index, msgpack::type::object_type type) const;

    /*!
     * Retrieves the number of fields in the message.
     *
     * @return The number of fields in the message.
     */
    std::size_t size() const;

    /*!
     * The message fields.
     *
     * @return The message fields.
     */
    const message_fields& fields() const;

    /*!
     * Pilfers the message fields.
     *
     * @return The message fields.
     */
    message_fields&& fields();

    /*!
     * Pilfers the message zone.
     *
     * @return The message zone.
     */
    msgpack::zone&& zone();

private:
    /*!
     * The zone used to allocate message fields. The zone must outlive
     * the fields. If the fields are pilfered then the zone must also
     * be pilferred and stored along with the fields.
     */
    msgpack::zone m_zone;

    /*!
     * The fields comprising of the message. It is up to the user of this
     * class to ensure that a valid wamp message has been constructed.
     */
    message_fields m_fields;
};

} // namespace autobahn

#include "wamp_message.ipp"

#endif // AUTOBAHN_WAMP_MESSAGE_HPP
