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
#include <boost/filesystem.hpp>


using namespace std;
using namespace boost;
using namespace autobahn;

using boost::asio::local::stream_protocol;


any add2(const anyvec& args, const anymap& kwargs) {

   cerr << "Someone is calling add2() .." << endl;

   uint64_t x = any_cast<uint64_t> (args[0]);
   uint64_t y = any_cast<uint64_t> (args[1]);
   return x + y;
}



int main (int argc, char** argv) {

   if (argc < 2) {
      cerr << "Usage: program <Unix domain socket path>" << endl;
      return 1;
   }

   // read config from stdin .. this will block until EOF is read.
   string stdin_contents((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());

   cerr << "Read " << stdin_contents.size() << " bytes from stdin" << endl;

   cerr << "Running on Boost version " << BOOST_VERSION << endl;

   try {
      // ASIO service object
      //
      asio::io_service io;

      // the Unix domain socket we connect
      //
      stream_protocol::socket socket(io);
      socket.connect(stream_protocol::endpoint(argv[1]));
      cerr << "Connected to WAMP router." << endl;

      // create a WAMP session that talks over TCP
      //
      bool debug = false;
      autobahn::session<stream_protocol::socket,
                        stream_protocol::socket> session(io, socket, socket, debug);

      // start the WAMP session on the transport that has been connected
      //
      session.start();

      // join a realm with the WAMP session
      //
      auto session_future = session.join("realm1").then([&](future<uint64_t> s) {

         cerr << "Session joined to realm with session ID " << s.get() << endl;

         // register a free standing function for remoting
         //
         auto r1 = session.provide("com.myapp.cpp.add2", &add2);

         r1.then([](future<registration> reg) {
            cerr << "Registered with registration ID " << reg.get().id << endl;
         }).wait();


         // register a lambda for remoting
         //
         session.provide("com.myapp.cpp.square",

            [](const anyvec& args, const anymap& kwargs) {

               cerr << "Someone is calling my lambda function .." << endl;

               uint64_t x = any_cast<uint64_t> (args[0]);
               return x * x;
            }
         ).wait();
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
