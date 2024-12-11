#pragma once

#include <cstdint>
#include <format>
#include <iostream>
#include <string>
#include <tuple>

using color = uint32_t;
using uchar = unsigned char;

inline auto rgb( color c ) -> std::tuple<int, int, int>
{
  auto value = static_cast<uint32_t>( c );
  int r = ( value >> 16 ) & 0xFF;
  int g = ( value >> 8 ) & 0xFF;
  int b = value & 0xFF;
  return std::make_tuple( r, g, b );
}

inline auto bg( color c ) -> std::string
{
  auto [r, g, b] = rgb( c );
  return std::format( "\033[48;2;{};{};{}m", r, g, b );
}
// front groud
inline auto fg( color c ) -> std::string
{
  auto [r, g, b] = rgb( c );
  return std::format( "\033[38;2;{};{};{}m", r, g, b );
}

inline color Color( uchar r, uchar g, uchar b )
{
  return r << 16 | g << 8 | b;
}

inline constexpr auto end_color()
{
  return "\033[0m";
}

inline auto print( std::string_view content, color fc = 0xffffffff, color bc = 0 ) -> void
{
  std::cout << fg( fc ) << bg( bc ) << content << end_color();
}
