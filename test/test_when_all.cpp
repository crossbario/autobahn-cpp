
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

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>


using namespace boost;
using namespace std;


int main() {
   future<int> f1 = async([]() { return 1; });
   //future<int> f2 = async([]() { return 2; });
   //future<int> f3 = async([]() { return 3; });

   auto f1_ = f1.then([](decltype(f1) res) {
      cout << "f1 done" << endl;
      int ires = res.get();
      auto fi = async([=]() { return 11 + ires; });
      //return fi.get();
      //return 11 + ires;
      return async([=]() { return 11 + ires; });
   });

   int fres = f1_.get().get();
//   int fres = f1_.unwrap().get();
//   int fres = f1_.get();
   cout << "fres done" << fres << endl;

/*
   auto f1_d = f1_.then([](decltype(f1_)) {
      std::cout << "f1_ done" << std::endl;
   });

   f1_d.get();

   auto f2_ = f2.then([](decltype(f2) res) {
      std::cout << "f2 done" << std::endl;
   });

   auto f3_ = f3.then([](decltype(f3) res) {
      std::cout << "f3 done" << std::endl;
   });

   auto f12 = when_all(std::move(f1), std::move(f2));

   auto f12d = f12.then([](decltype(f12)) {
      std::cout << "f12 done" << std::endl;
   });

   auto f23 = when_all(std::move(f2), std::move(f3));

   auto f23d = f23.then([](decltype(f23)) {
      std::cout << "f23 done" << std::endl;
   });

   auto f123 = when_all(std::move(f12d), std::move(f23d));

   auto fall = f123.then([](decltype(f123)) {
      std::cout << "all done" << std::endl;
   });

   fall.get();
*/
}
