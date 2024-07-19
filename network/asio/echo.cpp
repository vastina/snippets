#include <asio.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <system_error>

using asio::ip::tcp;
using u64 = std::uint64_t;

class sock : public std::enable_shared_from_this<sock>
{
private:
  tcp::socket so_;
  constexpr static u64 max_len { 1024u };
  char data_[max_len];

private:
  void do_write( u64 length )
  {
    auto self { shared_from_this() };
    so_.async_write_some( asio::buffer( data_, length ), [this, self]( std::error_code ec, u64 ) {
      if ( !ec )
        do_read();
    } );
  }
  void do_read()
  {
    auto self { shared_from_this() };
    so_.async_read_some( asio::buffer( data_, max_len ), [this, self]( std::error_code ec, u64 length ) {
      // clang-format off
std::cout << "recive msg: " << data_ << '\n';
      // clang-format on
      if ( !ec )
        do_write( length );
    } );
  }

public:
  sock( tcp::socket socket ) : so_( std::move( socket ) ) {}
  void start() { do_read(); }
};

class echoSever
{
private:
  tcp::socket sock_;
  tcp::acceptor acp_;
  void do_accept()
  {
    acp_.async_accept( sock_, [this]( std::error_code ec ) {
      if ( !ec )
        std::make_shared<sock>( std::move( sock_ ) )->start();
      do_accept();
    } );
  }

public:
  echoSever( asio::io_context& ic, short port ) : acp_( ic, tcp::endpoint( tcp::v4(), port ) ), sock_( ic )
  {
    do_accept();
  }
};

asio::io_context ic;
int main( int argc, char** argv )
{
  ::signal( SIGINT, []( int ) {
    ic.stop();
    std::clog << "sig int\n";
    std::exit( 0 );
  } );
  ::signal( SIGQUIT, []( int ) {
    ic.stop();
    std::clog << "sig quit\n";
    std::exit( 0 );
  } );
  try {
    echoSever s( ic, argc > 1 ? std::atoi( argv[1] ) : 1145 );

    ic.run();
  } catch ( const std::exception e ) {
    std::cout << e.what() << '\n';
    return 1;
  }

  return 0;
}