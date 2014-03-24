
// clang++ -o test8 -std=c++11 -stdlib=libc++ -lboost_thread -lboost_system -I/home/oberstet/boost_1_55_0 -L/home/oberstet/boost_1_55_0/stage/lib test8.cpp

#include <iostream>

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>


#define PD


struct Foo {

#ifdef PD
   boost::future<int> start() {
      boost::future<int> f = p.get_future();
      f.set_deferred();
      return f;
   }
#else
   boost::future<int> start() {
      return p.get_future();
   }
#endif

   void finish() {
      p.set_value(666);
   }

   boost::promise<int> p;
};

#define V0
//#define V1
//#define V2
//#define V3

int main () {

   Foo foo;

#ifdef V0
   // does ONLY work with PD set

   foo.start().then([](boost::future<int> f) {
      std::cout << "done:" << std::endl;
      std::cout << f.get() << std::endl;
   });
#endif

#ifdef V1
   // this DOES work

   foo.start().then(boost::launch::deferred, [](boost::future<int> f) {
      std::cout << "done:" << std::endl;
      std::cout << f.get() << std::endl;
   });
#endif

#ifdef V2
   // does NOT work

   boost::future<int> f0 = foo.start();
   f0.then([](boost::future<int> f) {
      std::cout << "done:" << std::endl;
      std::cout << f.get() << std::endl;
   });
#endif

#ifdef V3
   // this DOES work

   boost::future<void> f2 = foo.start().then([](boost::future<int> f) {
      std::cout << "done:" << std::endl;
      std::cout << f.get() << std::endl;
   });
#endif

   foo.finish();

   //boost::chrono::milliseconds duration(1000);
   //boost::this_thread::sleep_for(duration);
}
