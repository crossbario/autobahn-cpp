#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <cstdlib>
#include <cstring>

void print(const boost::system::error_code& /*e*/) {
   std::cout << "Hello, world!\n";
}

void print2() {
   std::cout << "print2" << std::endl;
}

// http://en.highscore.de/cpp/boost/asio.html#asio_extensions
// http://stackoverflow.com/questions/7855222/how-to-asynchronously-read-input-from-command-line-using-boost-asio-in-windows
// http://stackoverflow.com/questions/2831253/using-boostasioasync-read-with-stdin

using namespace boost;

#include <string>
#include <cstdint>


#include <arpa/inet.h>
#include <unistd.h>

#include <msgpack.hpp>


template<typename IStream, typename OStream>
class transport {

   public:
      transport(IStream& in, OStream& out)
         : m_debug(true),
           m_stdin(in),
           m_stdout(out)
      {}


      void test() {

         receive_msg();
      }

      void close() {
         m_stdin.close();
         m_stdout.close();
      }

   private:

      // boost::asio::async_write(s, boost::asio::buffer(data, size), handler);
      // boost::asio::write(s, boost::asio::buffer(data, size));


      void receive_msg() {

         // read 4 octets msg length prefix ..
         asio::async_read(m_stdin,
            asio::buffer(m_buffer_msg_len, sizeof(m_buffer_msg_len)),
            bind(&transport::got_msg_header, this, asio::placeholders::error));
      }

      void got_msg_header(const system::error_code& error) {
         if (!error) {

            m_msg_len = ntohl(*((uint32_t*) &m_buffer_msg_len));

            if (m_debug) {
               std::cerr << "RX message (" << m_msg_len << " octets) ..." << std::endl;
            }

            // read actual message
            m_unpacker.reserve_buffer(m_msg_len);

            asio::async_read(m_stdin,
               asio::buffer(m_unpacker.buffer(), m_msg_len),
               bind(&transport::got_msg_body, this, asio::placeholders::error));

         } else {
         }
      }

      void got_msg_body(const system::error_code& error) {
         if (!error) {

            if (m_debug) {
               std::cerr << "RX message received." << std::endl;
            }

            m_unpacker.buffer_consumed(m_msg_len);

            msgpack::unpacked result;

            while (m_unpacker.next(&result)) {

               msgpack::object obj(result.get());

               if (m_debug) {
                  std::cout << "RX WAMP message: " << obj << std::endl;
               }
            }

            receive_msg();

         } else {

         }
      }


      bool m_debug;

      char m_buffer_msg_len[4];
      uint32_t m_msg_len;

      IStream& m_stdin;
      OStream& m_stdout;

      /// MsgPack unserialization unpacker.
      msgpack::unpacker m_unpacker;
};



void handle_read_input(const boost::system::error_code& error, std::size_t length) {
   std::cout << "print3" << std::endl;
/*
    if (!error)
    {
      // Write the message (minus the newline) to the server.
      write_msg_.body_length(length - 1);
      input_buffer_.sgetn(write_msg_.body(), length - 1);
      input_buffer_.consume(1); // Remove newline from input.
      write_msg_.encode_header();
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msg_.data(), write_msg_.length()),
          boost::bind(&posix_chat_client::handle_write, this,
            boost::asio::placeholders::error));
    }
    else if (error == boost::asio::error::not_found)
    {
      // Didn't get a newline. Send whatever we have.
      write_msg_.body_length(input_buffer_.size());
      input_buffer_.sgetn(write_msg_.body(), input_buffer_.size());
      write_msg_.encode_header();
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msg_.data(), write_msg_.length()),
          boost::bind(&posix_chat_client::handle_write, this,
            boost::asio::placeholders::error));
    }
    else
    {
      close();
    }
    */
 }


int main () {
   try {
      asio::io_service io;

      asio::posix::stream_descriptor in(io, ::dup(STDIN_FILENO));
      asio::posix::stream_descriptor out(io, ::dup(STDOUT_FILENO));

      //auto trnsp = transport(in, out);
      transport<asio::posix::stream_descriptor, asio::posix::stream_descriptor> trnsp(in, out);
      trnsp.test();

/*
      io_service.post(&print2);

      boost::asio::deadline_timer t(io_service, boost::posix_time::seconds(1));
      t.async_wait(&print);


      asio::posix::stream_descriptor input(io_service, ::dup(STDIN_FILENO));
      asio::posix::stream_descriptor output(io_service, ::dup(STDOUT_FILENO));
      asio::streambuf buffer(1000);

      asio::async_read_until(input, buffer, '\n',
          bind(&handle_read_input,
            asio::placeholders::error,
            asio::placeholders::bytes_transferred));
*/

      io.run();
   }
   catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
   }
   return 0;
}
