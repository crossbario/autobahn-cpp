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
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include <autobahn/autobahn.hpp>
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>

using namespace std;
using namespace boost;
using namespace autobahn;

using boost::asio::ip::tcp;

int main () {

   cerr << "Running on " << BOOST_VERSION << endl;

   try {
      // ASIO service object
      //
      asio::io_service io;

      // the TCP socket we connect
      //
      tcp::socket socket(io);

      // connect to this server/port
      //
      tcp::resolver resolver(io);
      auto endpoint_iterator = resolver.resolve({"127.0.0.1", "8090"});

      // create a WAMP session that talks over TCP
      //
      bool debug = false;
      autobahn::wamp_session<tcp::socket,
                        tcp::socket> session(io, socket, socket, debug);

      // make sure the future returned from the session joining a realm (see below)
      // does not run out of scope (being destructed prematurely ..)
      //
      future<void> session_future;

      // now do an asynchronous connect ..
      //
      boost::asio::async_connect(socket, endpoint_iterator,

         // we either connected or an error happened during connect ..
         //
         [&](boost::system::error_code ec, tcp::resolver::iterator) {

            if (!ec) {
               cerr << "Connected to server" << endl;

               // start the WAMP session on the transport that has been connected
               //
               session.start();

               // join a realm with the WAMP session
               //
               session_future = session.join("realm1").then([&](future<uint64_t> s) {

                  cerr << "Session joined to realm with session ID " << s.get() << endl;

                  // issue a number of remote procedure calls ..
                  //
                  std::tuple<uint64_t, uint64_t> c0_args(2, 9);
                  auto c0 = session.call("com.mathservice.add2", c0_args)
                     .then([](future<wamp_call_result> result) {
                        std::tuple<uint64_t> result_arguments;
                        result.get().arguments().convert(result_arguments);
                        cerr << "Call 0 result: " << std::get<0>(result_arguments) << endl;
                  });

                  c0.wait();

                  std::tuple<uint64_t> c1_args(2);
                  std::unordered_map<std::string, uint64_t> c1_kw_args = {{"delay", 3}};
                  auto c1 = session.call("com.math.slowsquare", c1_args, c1_kw_args)
                     .then([](future<wamp_call_result> result) {
                        std::tuple<uint64_t> result_arguments;
                        result.get().arguments().convert(result_arguments);
                        cerr << "Call 1 result: " << std::get<0>(result_arguments) << endl;
                  });

                  std::tuple<uint64_t> c2_args(3);
                  auto c2 = session.call("com.math.slowsquare", c2_args)
                     .then([&session](future<wamp_call_result> result) {
                        std::tuple<uint64_t> result_arguments;
                        result.get().arguments().convert(result_arguments);
                        cerr << "Call 2 result: " << std::get<0>(result_arguments) << endl;
                  });

                  // do something when all remote procedure calls have finished
                  //
                  auto finish = when_all(std::move(c0), std::move(c1), std::move(c2));

                  finish.then([&](decltype(finish)) {

                     cerr << "All calls finished" << endl;

                     // leave the session and stop I/O loop
                     //
                     session.leave().then([&](future<string> reason) {
                        cerr << "Session left (" << reason.get() << ")" << endl;
                        io.stop();
                     }).wait();
                  });
               });

            } else {
               cerr << "Could not connect to server: " << ec.message() << endl;
            }
         }
      );

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
