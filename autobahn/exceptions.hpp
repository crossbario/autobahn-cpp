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

#ifndef AUTOBAHN_EXCEPTIONS_HPP
#define AUTOBAHN_EXCEPTIONS_HPP

namespace autobahn {

class protocol_error : public std::runtime_error {
  public:
     protocol_error(const std::string& message) : std::runtime_error(message) {};
};

class no_session_error : public std::runtime_error {
  public:
     no_session_error() : std::runtime_error("session not joined") {};
};

} // namespace autobahn

#endif // AUTOBAHN_EXCEPTIONS_HPP
