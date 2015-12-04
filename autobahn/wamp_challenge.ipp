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

namespace autobahn {

inline wamp_challenge::wamp_challenge( 
        const std::string & authmethod,
        const std::string & challenge,
        const std::string & salt,
        int iterations,
        int keylen)
: m_authmethod( authmethod )
, m_challenge( challenge )
, m_salt( salt )
, m_iterations( iterations )
, m_keylen( keylen )
{}

inline wamp_challenge::wamp_challenge( const wamp_challenge & ch )
{
    m_authmethod = ch.authmethod();
    m_challenge = ch.challenge();
    m_salt = ch.salt();
    m_iterations = ch.iterations();
    m_keylen = ch.keylen();
}


inline const std::string & wamp_challenge::authmethod() const { return m_authmethod; }
inline const std::string & wamp_challenge::challenge() const { return m_challenge; }
inline const std::string & wamp_challenge::salt() const { return m_salt; }
inline int wamp_challenge::iterations() const { return m_iterations; }
inline int wamp_challenge::keylen() const { return m_keylen; }


} // namespace autobahn
