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
#include <boost/asio.hpp>

#include "autobahn.hpp"

using namespace std;
using namespace boost;


void print(const boost::system::error_code&) {
   cerr << "Hello from worker timer" << endl;
}


int main () {

   try {
      asio::io_service io;
      //asio::io_service::work work(io);

      boost::asio::deadline_timer t(io, posix_time::seconds(5));
      t.async_wait(&print);

      asio::posix::stream_descriptor in(io, ::dup(STDIN_FILENO));
      asio::posix::stream_descriptor out(io, ::dup(STDOUT_FILENO));

      autobahn::session<asio::posix::stream_descriptor,
                        asio::posix::stream_descriptor> session(in, out);


      auto s = session.join(std::string("realm1")).then([&session](future<int> s) {
         cerr << "session joined" << endl;
         session.publish("com.myapp.topic1");
         session.foo();
         cerr << "session 1" << endl;

         // Publish an event
         //
//         session.publish("com.myapp.topic1", {1, string("zwei"), false}, {{"foo", 23}, {"bar", string("baz")}});

         auto call3 = session.call("com.mathservice.add2", {23, 7}).then([&session](future<any> f) {

            cerr << "call returned" << endl;

            uint64_t res = any_cast<uint64_t> (f.get());

            cerr << "Got result 3: " << res << endl;

         });

         cerr << "session 2" << endl;

         call3.wait();
      });

      cerr << "Starting I/O loop .." << endl;

      io.run();

      s.wait();

      cerr << "I/O loop ended .." << endl;
   }
   catch (std::exception& e) {
      cerr << e.what() << endl;
      return 1;
   }
   return 0;
}
