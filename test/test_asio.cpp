#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void print(const boost::system::error_code& /*e*/) {
   std::cout << "Hello, world!\n";
}

void print2() {
   std::cout << "print2" << std::endl;
}

// http://en.highscore.de/cpp/boost/asio.html#asio_extensions
// http://stackoverflow.com/questions/7855222/how-to-asynchronously-read-input-from-command-line-using-boost-asio-in-windows
// http://stackoverflow.com/questions/2831253/using-boostasioasync-read-with-stdin


int main () {
   try {
      boost::asio::io_service io_service;

      io_service.post(&print2);

      io_service.work_started();

      boost::asio::deadline_timer t(io_service, boost::posix_time::seconds(1));
      t.async_wait(&print);

      io_service.run();
   }
   catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
   }
   return 0;
}
