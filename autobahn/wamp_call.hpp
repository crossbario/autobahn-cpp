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

#ifndef AUTOBAHN_WAMP_CALL_HPP
#define AUTOBAHN_WAMP_CALL_HPP

#include "wamp_call_result.hpp"

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>

#include <msgpack.hpp>

namespace autobahn {

/// An outstanding wamp call.
class wamp_call
{
public:
    wamp_call();

    boost::promise<wamp_call_result>& result();
    void set_result(wamp_call_result&& value);

private:
    boost::promise<wamp_call_result> m_result;
};

} // namespace autobahn

#include "wamp_call.ipp"

#endif // AUTOBAHN_WAMP_CALL_HPP
