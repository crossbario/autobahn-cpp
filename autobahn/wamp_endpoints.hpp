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

#ifndef AUTOBAHN_WAMP_ENDPOINTS_HPP
#define AUTOBAHN_WAMP_ENDPOINTS_HPP

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/any.hpp>
#include <boost/thread/future.hpp>
#include <map>
#include <utility>
#include <vector>

namespace autobahn {

/// A map holding any values and string keys.
typedef std::map<std::string, boost::any> anymap;

/// A vector holding any values.
typedef std::vector<boost::any> anyvec;

/// A pair of ::anyvec and ::anymap.
typedef std::pair<anyvec, anymap> anyvecmap;

/// Handler type for use with session::subscribe(const std::string&, handler_t)
typedef std::function<void(const anyvec&, const anymap&)> handler_t;

/// Endpoint type for use with session::provide(const std::string&, endpoint_t)
typedef std::function<boost::any(const anyvec&, const anymap&)> endpoint_t;

/// Endpoint type for use with session::provide_v(const std::string&, endpoint_v_t)
typedef std::function<anyvec(const anyvec&, const anymap&)> endpoint_v_t;

/// Endpoint type for use with session::provide_m(const std::string&, endpoint_m_t)
typedef std::function<anymap(const anyvec&, const anymap&)> endpoint_m_t;

/// Endpoint type for use with session::provide_vm(const std::string&, endpoint_vm_t)
typedef std::function<anyvecmap(const anyvec&, const anymap&)> endpoint_vm_t;

/// Endpoint type for use with session::provide(const std::string&, endpoint_ft)
typedef std::function<boost::future<boost::any>(const anyvec&, const anymap&)> endpoint_f_t;

/// Endpoint type for use with session::provide_fv(const std::string&, endpoint_fv_t)
typedef std::function<boost::future<anyvec>(const anyvec&, const anymap&)> endpoint_fv_t;

/// Endpoint type for use with session::provide_fm(const std::string&, endpoint_fm_t)
typedef std::function<boost::future<anymap>(const anyvec&, const anymap&)> endpoint_fm_t;

/// Endpoint type for use with session::provide_fvm(const std::string&, endpoint_fvm_t)
typedef std::function<boost::future<anyvecmap>(const anyvec&, const anymap&)> endpoint_fvm_t;

typedef anymap provide_options;

} // namespace autobahn

#endif // AUTOBAHN_WAMP_ENDPOINTS_HPP
