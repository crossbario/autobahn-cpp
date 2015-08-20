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

#include <msgpack.hpp>
#include <string>
#include <unordered_map>

namespace autobahn {

inline wamp_call_options::wamp_call_options()
    : m_timeout()
{
}

inline const std::chrono::milliseconds& wamp_call_options::timeout() const
{
    return m_timeout;
}

inline void wamp_call_options::set_timeout(const std::chrono::milliseconds& timeout)
{
    m_timeout = timeout;
}

} // namespace autobahn

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template<>
struct convert<autobahn::wamp_call_options>
{
    msgpack::object const& operator()(
            msgpack::object const& object,
            autobahn::wamp_call_options& options) const
    {
        std::unordered_map<std::string, msgpack::object> options_map;
        object >> options_map;

        const auto options_map_itr = options_map.find("timeout");
        if (options_map_itr != options_map.end()) {
            options.set_timeout(std::chrono::milliseconds(options_map_itr->second.as<unsigned>()));
        }

        return object;
    }
};

template<>
struct pack<autobahn::wamp_call_options>
{
    template <typename Stream>
    msgpack::packer<Stream>& operator()(
            msgpack::packer<Stream>& packer,
            autobahn::wamp_call_options const& options) const
    {
        std::unordered_map<std::string, unsigned> options_map;
        const auto& timeout = options.timeout();
        if (timeout.count() > 0) {
            options_map["timeout"] = timeout.count();
        }

        packer.pack(options_map);

        return packer;
    }
};

template <>
struct object_with_zone<autobahn::wamp_call_options>
{
    void operator()(
            msgpack::object::with_zone& object,
            const autobahn::wamp_call_options& options)
    {
        std::unordered_map<std::string, msgpack::object> options_map;

        const auto& timeout = options.timeout();
        if (timeout.count() != 0) {
            options_map["timeout"] = msgpack::object(timeout.count());
        }

        object << options_map;
    }
};

} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack
