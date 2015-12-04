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

#ifndef AUTOBAHN_WAMP_CHALLENGE_HPP
#define AUTOBAHN_WAMP_CHALLENGE_HPP

namespace autobahn {

/// Represents a topic subscription.
class wamp_challenge
{
public:
    wamp_challenge(
        const std::string & authmethod,
        const std::string & challenge = "",
        const std::string & salt = "",
        int iteratons = -1,
        int keylen = -1
       );

    wamp_challenge( const wamp_challenge & );

    const std::string & challenge() const;
    const std::string & authmethod() const;
    const std::string & salt() const;
    int iterations() const;
    int keylen() const;

    

private:
    // authmethod
    std::string m_authmethod;
    
    // used to "digest" the secret for transport
    std::string m_challenge;

    /////////////////////////////////////////
    // if authmethod is "wampcra"
    // and the secret is stored as a derived_key
    // with salt, iterations and a given keylen.
    std::string m_salt;
    int m_iterations;
    int m_keylen;
};

} // namespace autobahn

#include "wamp_challenge.ipp"

#endif // AUTOBAHN_WAMP_CHALLENGE_HPP
