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
#include <string>
#include <thread>

#include "autobahn.hpp"

#include <boost/asio.hpp>

using namespace std;
using namespace boost;

using boost::asio::ip::tcp;


int main () {

   try {
      asio::io_service io;

      asio::deadline_timer timer(io, posix_time::seconds(2));
      timer.async_wait(
         [](system::error_code ec) {
            if (!ec) {
               cerr << "Timeout!" << endl;
            } else {
               cerr << "Error in timer" << endl;
            }
         }
      );

#if 1
      tcp::socket socket(io);

      autobahn::session<tcp::socket,
                        tcp::socket> session(socket, socket);

      tcp::resolver resolver(io);
      auto endpoint_iterator = resolver.resolve({"127.0.0.1", "8080"});


      boost::asio::async_connect(socket, endpoint_iterator,

         [&](boost::system::error_code ec, tcp::resolver::iterator) {

            if (!ec) {
               cerr << "connected" << endl;
               session.start();

               auto s = session.join(string("realm1")).then([&session](future<int> s) {
                  cerr << "session joined" << endl;
                  session.publish("com.myapp.topic1");

#if 1
                  auto cf = session.call("com.arguments.add2", {2, 3});

                  auto c = cf.then([&session](future<any> f) {

                     cerr << "call returned" << endl;

                     any r = f.get();

                     cerr << "result type: " << r.type().name() << endl;

                     uint64_t res = any_cast<uint64_t> (r);

                     cerr << "result: " << res << endl;
                  });

                  //c.wait();
#endif
               });

            } else {
               cerr << "could not connect" << endl;
            }
         }
      );
#endif


#if 1
      io.run();
#else      
      std::thread t([&io](){ io.run(); });
      t.join();
#endif

      cerr << "I/O loop ended .." << endl;
   }
   catch (std::exception& e) {
      cerr << e.what() << endl;
      return 1;
   }
   return 0;
}
