#include "base/io.hpp"
#include "base/token.hpp"
#include "base/util.hpp"
#include "json.hpp"

#include <filesystem>
#include <iostream>

using namespace vastina;

int main( int argc, char** argv )
{
  if ( argc > 2 ) {
    std::cout << "too many arguments\n";
    return 1;
  }
  if ( argc != 2 ) {
    std::cout << "no file given\n";
    return 2;
  }
  const auto file_name { string_view( argv[1] ) };
  if ( !std::filesystem::exists( file_name ) ) {
    std::cout << "file not exists\n";
    return 3;
  }

  json::lexer lx { json::lexer( file_name ) };
  lx.Parse();

  const auto tks { lx.getTokens() };
  for ( u32 i = 0; i < tks.size(); i++ ) {
    print( "offset:{}, \ttoken:{} \ttokenid:{}\n", i, tks.at( i ).data_, (i32)tks.at( i ).token_ );
  }

  return 0;
}