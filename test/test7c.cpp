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

#include "autobahn.hpp"

using namespace boost;
using namespace std;
using namespace autobahn;


int main () {

   // A Worker MUST log to std::cerr, since std::cin/cout is used
   // for talking WAMP with the master
   //
   std::cerr << "Worker starting .." << std::endl;

   // Setup WAMP session running over stdio
   //
   autobahn::session session(std::cin, std::cout);

   // Launch policy to use
   //
   boost::launch lp = boost::launch::deferred;

   // To establish a session, we join a "realm" ..
   //
   session.join(std::string("realm1")).then([&](boost::future<int> fs) {

      // WAMP session is now established ..
      //
      int session_id = fs.get();

      std::cerr << "Joined with session ID " << session_id << std::endl;

      autobahn::anyvec params1 = {2, "foobar"};
      autobahn::anymap params2 = {{"a", 23}, {"b", 7}};


      // Variant A: positional args via generic vector, generic result
      //
      autobahn::anyvec args;
      args.push_back(23);
      args.push_back(777);
/*
      session.call("com.mathservice.add2", args).then(lp, [](boost::future<boost::any> f) {
         uint64_t res = boost::any_cast<uint64_t> (f.get());
         std::cerr << "A - Got RPC result " << res << std::endl;
      });
*/

#if 0
      // Variant B: generic positional args, generic result
      //
      boost::any a = 23;
      boost::any b = 777;

      session.call("com.mathservice.add2", a, b).then(lp, [](boost::future<boost::any> f) {
         int res = boost::any_cast<int> (f.get());
         std::cerr << "B - Got RPC result " << res << std::endl;
      });


      // Variant C: typed positional args, generic result
      //
      session.call("com.mathservice.add2", 23, 777).then(lp, [](boost::future<boost::any> f) {
         int res = boost::any_cast<int> (f.get());
         std::cerr << "C - Got RPC result " << res << std::endl;
      });

#endif
#if 0
      // Variant C: typed positional args, generic result
      //
      session.call("com.mathservice.add2", {23, 777}).then(lp, [](boost::future<boost::any> f) {
         int res = boost::any_cast<int> (f.get());
         std::cerr << "D - Got RPC result " << res << std::endl;
      });


      session.call("com.arguments.stars", {}, {{"stars", 10}}).then(lp, [](boost::future<boost::any> f) {
         std::string res = boost::any_cast<std::string> (f.get());
         std::cerr << "E - Got RPC result " << res << std::endl;
      });
#endif
/*
      session.call<std::string>("com.arguments.stars", {}, {{"stars", 20}}).then(lp, [](boost::future<std::string> f) {
         std::cerr << "F - Got RPC result " << f.get() << std::endl;
      });
*/
/*
      session.call("com.arguments.numbers", {1, 7}, {{"prefix", std::string("Hello number: ")}}).then([](boost::future<boost::any> f) {
//      session.call("com.arguments.numbers", {1, 7}).then([](boost::future<boost::any> f) {
         boost::any res = f.get();
         std::cerr << "G - Got RPC result " << res.type().name() << std::endl;

         autobahn::anyvec v = boost::any_cast<autobahn::anyvec>(res);
         std::cerr << "G2 " << v.size() << std::endl;
         for (int i = 0; i < v.size(); ++i) {
            std::cerr << boost::any_cast<std::string>(v[i]) << std::endl;
         }
      });
*/

/*
      auto f1 = session.call("com.mathservice.add2", {7, 33}).then(
         [](future<any> f) {

            uint64_t res = any_cast<uint64_t> (f.get());
            cerr << "A - Got RPC result " << res << endl;
            return res * 2;
         }
      ).then(lp, [](future<uint64_t> f) {
         cerr << "A DONE " << f.get() << endl;
      });
*/
/*
template<class Lhs, class Rhs>
  auto adding_func(const Lhs &lhs, const Rhs &rhs) -> decltype(lhs+rhs) {return lhs + rhs;}
*/
   auto xx = 33;

   decltype(xx) yy;

   yy = 2 * xx;
#if 0
   shared_future<int> shared_future1 = async([] { return 125; });

   future<std::string> future2 = async([]() { return std::string("hi"); });

   future<tuple<shared_future<int>,future<std::string>>> all_f = when_all(shared_future1, future2);

   future<int> result = all_f.then([](future<boost::tuple<shared_future<int>,future<std::string>>> f) {
      std::cerr << f.get().type().name() << std::endl;
   });
#endif

#if 1
      auto f1 = session.call("com.mathservice.add2", {7, 33});

      auto f1done = f1.then(lp, [](decltype(f1) res) {
         cerr << "Result 1: " << any_cast<uint64_t>(res.get()) << endl;
         return 10;
      });

      auto f2 = session.call("com.mathservice.add2", {60, 90});

      auto f2done = f2.then(lp, [](decltype(f2) res) {
         cerr << "Result 2: " << any_cast<uint64_t>(res.get()) << endl;
         return 20;
      });

      //future<tuple<shared_future<int>, future<int>>> f3 = when_all(f1done, f2done);
#endif
#if 0
      auto f3 = when_all(f1done, f2done);

      f3.then(lp, [](decltype(f3) res) {
         cerr << "Done." << endl;
      });

      when_all(f1done, f2done).then(lp, [](decltype(f3) res) {
         cerr << "Done." << endl;
      });
#else
#endif
/*
      wait_for_all(f1, f2);
      cerr << "Done." << endl;
*/
      //decltype(some_int)
/*
      auto f1 = session.call("com.mathservice.add2", {7, 33}).then(
         [](future<any> f) {

            uint64_t res = any_cast<uint64_t> (f.get());
            cerr << "Result 1: " << res << endl;
         }
      );

      auto f2 = session.call("com.mathservice.add2", {10, 40}).then(
         [](future<any> f) {

            uint64_t res = any_cast<uint64_t> (f.get());
            cerr << "Result 2: " << res << endl;
         }
      );

      wait_for_all(f1, f2).then(lp,
         [](boost::future<void>, boost::future<void>) {

            cerr << "Done." << endl;
         }
      );
*/
/*
//      boost::future<void> f2 = session.call("com.arguments.numbers", {1, 7}, {{"prefix", std::string("Hello number: ")}}).then(
      auto f2 = session.call("com.arguments.numbers", {1, 7}, {{"prefix", std::string("Hello number: ")}}).then(
         [](boost::future<boost::any> f) {

            boost::any res = f.get();
            std::cerr << "G - Got RPC result " << res.type().name() << std::endl;

            autobahn::anyvec v = boost::any_cast<autobahn::anyvec>(res);
            std::cerr << "G2 " << v.size() << std::endl;
            for (int i = 0; i < v.size(); ++i) {
               std::cerr << boost::any_cast<std::string>(v[i]) << std::endl;
            }
         }
      );
*/
/*
      f2.then(lp, [](boost::future<void>) {
         std::cerr << "DONE F2" << std::endl;
      });
*/
/*
      boost::wait_for_all(f1, f2).then(lp, [](boost::future<void>, boost::future<void>) {
         std::cerr << "DONE F2" << std::endl;
      });
*/
//      boost::future< std::tuple< boost::future<void>, boost::future<void> > > f3 = boost::wait_for_all(f1, f2);
      //auto f3 = boost::wait_for_all(f1, f2);

   });


   // Enter event loop for session ..
   //
   session.loop();
}
