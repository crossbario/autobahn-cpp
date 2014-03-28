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

#include <map>
#include <memory>
#include <iostream>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>
#include <boost/asio.hpp>

using namespace std;
using namespace boost;


struct rpcsvc {

   rpcsvc (asio::io_service& io) : m_io(io), m_call_id(0) {
   }

   struct Call {
      std::shared_ptr<asio::deadline_timer> m_timer;
      promise<float> m_promise;
   };

   future<float> slowsquare(float x, float delay) {

      m_call_id += 1;
      m_calls[m_call_id] = Call();

      std::shared_ptr<asio::deadline_timer>
         timer(new asio::deadline_timer(m_io, posix_time::seconds(delay)));

      m_calls[m_call_id].m_timer = timer;

      int call_id = m_call_id;

      timer->async_wait(
         [=](system::error_code ec) {           
            if (!ec) {

               // Question 1:
               //
               // m_call_id seems to be captured by value, though we
               // said [=] in the lambda .. why? We workaround by
               // using a local var in the enclosing method body.
               //
               cout << m_call_id << " - " << call_id << endl;

               this->m_calls[call_id].m_promise.set_value(x * x);
               this->m_calls.erase(call_id);
            } else {
               cout << "Error in timer: " << ec << endl;
            }
         }
      );

      cout << "call " << m_call_id << " issued" << endl;

      return m_calls[m_call_id].m_promise.get_future();
   }

   asio::io_service& m_io;
   int m_call_id;
   map<int, Call> m_calls;
};


int main () {

   try {
      asio::io_service io;

      rpcsvc rpc(io);

      auto f1 = rpc.slowsquare(2, 2).then([](future<float> f) {
         cout << "call 1 returned" << endl;
         cout << "result 1: " << f.get() << endl;
      });

      auto f2 = rpc.slowsquare(3, 1.1).then([](future<float> f) {
         cout << "call 2 returned" << endl;
         cout << "result 2: " << f.get() << endl;
      });

      auto f3 = rpc.slowsquare(4, 1).then([](future<float> f) {
         cout << "call 3 returned" << endl;
         cout << "result 3: " << f.get() << endl;
      });

      auto f12 = when_all(std::move(f1), std::move(f2));
      auto f12d = f12.then([](decltype(f12)) {
         cout << "call 1/2 done" << endl;
      });

/*
      auto f23 = when_all(std::move(f2), std::move(f3));
      auto f23d = f23.then([](decltype(f23)) {
         cout << "call 2/3 done" << endl;
      });

      auto f123 = when_all(std::move(f12d), std::move(f23d));
      auto f123d = f123.then([](decltype(f123)) {
         cout << "all calls done" << endl;
      });
*/
      io.run();

      // Question 2:
      //
      // Neither f12 nor f12d (and etc) "induce" any work on
      // ASIO reactor .. so ASIO will end it's loop though the
      // continuation hasn't been executed yet. we workaround
      // by "manually" waiting .. but that seems suboptimal ..
      //
      f12d.get();


      // Question 3:
      //
      // When above f23,f23d,f123,f123d and the line below
      // is commented in, the program will deadlock somehow.
      // Why? And is there a safe ("fool proof") way to avoid
      // this kind of issues?
      //f123d.get();
   }
   catch (std::exception& e) {
      cerr << e.what() << endl;
      return 1;
   }
   return 0;
}
