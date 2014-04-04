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

#include <string>
#include <iostream>

#include "autobahn.hpp"

#include <boost/asio.hpp>
#include <boost/version.hpp>

using namespace std;
using namespace boost;
using namespace autobahn;

using boost::asio::ip::tcp;


/// Procedure that returns a single positional result (being a vector)
any numbers(const anyvec& args, const anymap& kwargs) {

   cerr << "Someone is calling numbers() .." << endl;

   uint64_t start = any_cast<uint64_t> (args[0]);
   uint64_t end = any_cast<uint64_t> (args[1]);

   anyvec res;
   for (uint64_t i = start; i < end; ++i) {
      res.push_back(i);
   }
   return res;
}


/// Procedure that returns 3 positional results (each being a scalar)
anyvec add_diff_mul(const anyvec& args, const anymap& kwargs) {

   cerr << "Someone is calling add_diff_mul() .." << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);

   return {x + y, x > y ? x - y : y - x, x * y};
}


/// Procedure that returns a future which resolves with both positional and keyword results
future<anyvecmap> somemath(const anyvec& args, const anymap& kwargs) {

   cerr << "Someone is calling somemath() .." << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);
   return make_ready_future(std::make_pair(anyvec({x + y, 2 * x}), anymap({{"foo", 23}, {"bar", string("baz")}})));
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
      autobahn::session<tcp::socket,
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
                     .then([](future<registration> reg) {
                        cerr << "Registered numbers() with registration ID " << reg.get().id << endl;
                     }
                  );

                  auto r2 = session.provide_v("com.myapp.cpp.add_diff_mul", &add_diff_mul)
                     .then([](future<registration> reg) {
                        cerr << "Registered add_diff_mul() with registration ID " << reg.get().id << endl;
                     }
                  );

                  auto r3 = session.provide_fvm("com.myapp.cpp.somemath", &somemath)
                     .then([](future<registration> reg) {
                        cerr << "Registered somemath() with registration ID " << reg.get().id << endl;
                     }
                  );

                  // do something when we are finished with all registrations ..
                  //
                  auto done = when_all(std::move(r1), std::move(r2), std::move(r3));

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
