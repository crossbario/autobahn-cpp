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

// Using boost:future with boost::when_all
// See also: http://stackoverflow.com/questions/22664296/using-boostfuture-with-continuations-and-boostwhen-all


#include <iostream>
#include <vector>

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <boost/thread/future.hpp>

using namespace boost;


int main() {
   auto f1 = async([]() { return 1; });
   auto f2 = async([]() { return 2; });
   auto f3 = async([]() { return 3; });

   f1.then([](decltype(f1)) {
      std::cout << "f1 done" << std::endl;
   });

   f2.then([](decltype(f2)) {
      std::cout << "f2 done" << std::endl;
   });

   f3.then([](decltype(f3)) {
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
}
