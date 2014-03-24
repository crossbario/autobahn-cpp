
// clang++ -o test8 -std=c++11 -stdlib=libc++ -lboost_thread -lboost_system -I/home/oberstet/boost_1_55_0 -L/home/oberstet/boost_1_55_0/stage/lib test8.cpp

#include <iostream>

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>


struct Foo {

   boost::future<int> start() {
      return p.get_future();
   }

   void finish() {
      p.set_value(666);
   }

   boost::promise<int> p;
};


int main () {

   Foo foo;

   foo.start().then([](boost::future<int> f) {
      std::cout << "done:" << std::endl;
      std::cout << f.get() << std::endl;
   });

   foo.finish();
}
