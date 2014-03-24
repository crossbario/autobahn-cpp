#if 0
boost::future<int> join2(const std::string& realm) {
   boost::promise<int> p;
   p.set_value(23);
   return p.get_future();
}

int main () {
   boost::future<int> f = join2("realm1");

   f.then([](boost::future<int> f) {
      // here .get() won't block
      std::cout << f.get() << std::endl;
   });

   // here, I'd like to use f.then(..)
   //f.wait();
   //std::cout << f.get() << std::endl;
}
#else

#include <iostream>
#include <string>

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#include <boost/thread/future.hpp>

using namespace boost;

int main() {
   future<int> f1 = async([]() { return 123; });
   future<std::string> f2 = f1.then([](future<int> f) {
      std::cout << f.get() << std::endl; // here .get() won't block
      return std::string("sgfsdfs");
   });
}

#endif
