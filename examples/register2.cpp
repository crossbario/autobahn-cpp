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

using boost::asio::ip::tcp;

/// Procedure that returns a single positional result (being a vector)
void numbers(wamp_invocation_context& context)
{
   cerr << "Someone is calling numbers() .." << endl;
   msgpack::type::tuple<uint64_t, uint64_t> arguments;
   context.arguments().convert(arguments);

   uint64_t start = arguments.get<0>();
   uint64_t end = arguments.get<1>();

   std::vector<uint64_t> result;
   for (uint64_t i = start; i < end; ++i) {
      result.push_back(i);
   }

   context.result().set_arguments(result);
}


/// Procedure that returns 3 positional results (each being a scalar)
void add_diff_mul(wamp_invocation_context& context)
{
   cerr << "Someone is calling add_diff_mul() .." << endl;
   msgpack::type::tuple<uint64_t, uint64_t> arguments;
   context.arguments().convert(arguments);

   uint64_t x = arguments.get<0>();
   uint64_t y = arguments.get<1>();

   msgpack::type::tuple<uint64_t, uint64_t, uint64_t> result(
         x + y, x > y ? x - y : y - x, x * y);
    context.result().set_arguments(result);
}

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

                  // register some procedure for remote calling ..
                  //
                  auto r1 = session.provide("com.myapp.cpp.numbers", &numbers)
                     .then([](future<wamp_registration> reg) {
                        cerr << "Registered numbers() with registration ID " << reg.get().id() << endl;
                     }
                  );

                  auto r2 = session.provide("com.myapp.cpp.add_diff_mul", &add_diff_mul)
                     .then([](future<wamp_registration> reg) {
                        cerr << "Registered add_diff_mul() with registration ID " << reg.get().id() << endl;
                     }
                  );

                  // do something when we are finished with all registrations ..
                  //
                  auto done = when_all(std::move(r1), std::move(r2));

                  done.then([](decltype(done)) {

                     cerr << "All procedures registered" << endl;
                  }).wait();
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
