#include <asio.hpp>
#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <csignal>
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

using asio::ip::tcp;

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
    tcp::socket so { ic };
    tcp::resolver resolver { ic };
    asio::connect( so, resolver.resolve( argc > 1 ? argv[1] : "127.0.0.1", argc > 2 ? argv[2] : "1145" ) );

    std::string req {};
    while ( true ) {
      std::cout << "send msg> ";
      std::getline( std::cin, req );
      asio::write( so, asio::buffer( req ) );

      char reply[BUFSIZ];
      size_t size = asio::read( so, asio::buffer( reply, req.size() ) );
      reply[size] = 0;
      std::cout << "Reply is: " << reply << '\n';
    }
  } catch ( const std::exception e ) {
    std::cout << e.what();
    return 1;
  }
}