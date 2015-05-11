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

#include <autobahn/autobahn.hpp>
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <msgpack.hpp>
#include <string>

using namespace std;
using namespace boost;
using namespace autobahn;

int main () {

   cerr << "Running on " << BOOST_VERSION << endl;

   try {
      // ASIO service object
      //
      asio::io_service io;

      asio::posix::stream_descriptor in(io, ::dup(STDIN_FILENO));
      asio::posix::stream_descriptor out(io, ::dup(STDOUT_FILENO));

      // create a WAMP session that talks over TCP
      //
      bool debug = false;
      autobahn::wamp_session<asio::posix::stream_descriptor,
                        asio::posix::stream_descriptor> session(io, in, out, debug);

      // start the WAMP session on the transport that has been connected
      //
      session.start();

      cerr << "Connected to server" << endl;

      // join a realm with the WAMP session
      //
      auto session_future = session.join("realm1").then([&](future<uint64_t> s) {

         cerr << "Session joined to realm with session ID " << s.get() << endl;

         // call a remote procedure ..
         //
         msgpack::type::tuple<uint64_t, uint64_t> arguments(23, 777);
         auto c1 = session.call("com.mathservice.add2", arguments).then(
            [&](future<wamp_call_result> result) {
               // call result received
               //
               msgpack::type::tuple<uint64_t> result_arguments;
                     result.get().arguments().convert(result_arguments);
               std::cerr << "Got RPC result " << result_arguments.get<0>() << std::endl;
         });

         c1.then([&](decltype(c1)) {
            // leave the session and stop I/O loop
            //
            session.leave().then([&](future<string> reason) {
               cerr << "Session left (" << reason.get() << ")" << endl;
               io.stop();
            }).wait();
         });
      });

      cerr << "Starting ASIO I/O loop .." << endl;

      io.run();

      cerr << "ASIO I/O loop ended" << endl;
   }
   catch (std::exception& e) {
      cerr << e.what() << endl;
      return 1;
   }
   return 0;
}
