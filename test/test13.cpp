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


#include <iostream>
#include <chrono>
#include <functional>

#include "autobahn.hpp"

#include <boost/asio.hpp>

using namespace std;
using namespace boost;

using boost::asio::ip::tcp;


int main () {

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
      auto endpoint_iterator = resolver.resolve({"127.0.0.1", "8080"});

      // create a WAMP session that talks over TCP
      //
      autobahn::session<tcp::socket,
                        tcp::socket> session(io, socket, socket);

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

                  // publish an event every second ..
                  //
                  bool stop_publish = false;
                  asio::deadline_timer timer(io, posix_time::seconds(1));

                  std::function<void ()> dopub = [&]() {
                     timer.async_wait([&](system::error_code) {

                        session.publish("com.myapp.topic1");

                        cerr << "Event published." << endl;

                        if (!stop_publish) {
                           timer.expires_at(timer.expires_at() + posix_time::seconds(1));
                           dopub();                           
                        }
                     });             
                  };
                  dopub();

                  // issue a number of remote procedure calls ..
                  //
                  auto c1 = session.call("com.math.slowsquare", {2}, {{"delay", 3}})
                     .then([](future<any> f) {

                     uint64_t res = any_cast<uint64_t> (f.get());
                     cerr << "Call 1 result: " << res << endl;
                  });

                  auto c2 = session.call("com.math.slowsquare", {3})
                     .then([&session](future<any> f) {

                     uint64_t res = any_cast<uint64_t> (f.get());
                     cerr << "Call 2 result: " << res << endl;

                     auto c3 = session.call("com.math.slowsquare", {4}, {{"delay", 10}})
                        .then([](future<any> f) {

                        uint64_t res = any_cast<uint64_t> (f.get());
                        cerr << "Call 3 result: " << res << endl;
                     });
                     c3.wait();
                  });

                  // do something when all remote procedure calls have finished
                  //
                  auto done = when_all(std::move(c1), std::move(c2));
                  done.then([&](decltype(done)) {
                     cerr << "All calls finished" << endl;
                     stop_publish = true;
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
