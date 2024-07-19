#include "base/token.hpp"

#include "base/util.hpp"
#include <stdexcept>
#include <sys/stat.h>

namespace vastina {
namespace json {

token_t::token_t( TOKEN tk, const string_view& data /*, u32 line, u32 lineoffset*/ )
  : token_( tk ), data_( data ) /*, line_( line ), lineoffset_( lineoffset ) */
{}

lexer::lexer( const string_view& filename )
{
  struct stat file_stat;
  ::stat( filename.data(), &file_stat );

  auto fs { std::fstream() };
  fs.open( filename.data() );
  buffer_.resize( file_stat.st_size );
  fs.readsome( buffer_.data(), file_stat.st_size );
  fs.close();
}

lexer::~lexer()
{
  tokens_.clear();
  buffer_.clear();
}

lexer::STATE lexer::ParseWhiteSpace()
{
  while ( isWhiteSpace( buffer_[offset_] ) ) {
    offset_++;
    if ( offset_ >= buffer_.size() )
      return STATE::END;
  }
  return STATE::NORMAL;
}

lexer::STATE lexer::NextLine()
{
  while ( '\n' != buffer_[offset_] ) {
    offset_++;
    if ( offset_ >= buffer_.size() )
      return STATE::END;
  }
  return STATE::NORMAL;
}

i32 lexer::ParseNext( TOKEN target, const std::function<bool( char )>& Endjudge, u32 except_len )
{
  u32 last_offset { offset_ };
  while ( Endjudge( buffer_[offset_] ) )
    offset_++;
  if ( offset_ - last_offset != except_len && except_len > 0 )
    return -1;
  tokens_.push_back( token_t( target, { buffer_.data() + last_offset, offset_ - last_offset } ) );
  return 0;
}

void lexer::forSingelWord( TOKEN target )
{
  // assert
  tokens_.push_back( token_t( target, { buffer_.data() + offset_, 1 } ) );
  offset_++;
}

i32 lexer::ParseNumber()
{
  u32 last_offset { offset_ };

  bool dot { false }; //.
  bool exp { false }; // e E
  bool zer { false };

  if ( buffer_[offset_] == '0' )
    zer = true;
  offset_++;

  while ( true ) {
    switch ( buffer_[offset_] ) {
      case '.':
        if ( dot || exp )
          return -1; // . after e and more than one . is invalid
        dot = true;
      case '0' ... '9':
        if ( zer && !exp && !dot )
          return -1;
        break;
      case 'e':
      case 'E': {
        if ( dot && buffer_[offset_ - 1] == '.' )
          return -1; // 233.e and more than one e is invalid
        if ( exp )
          return -1;
        exp = true;
        if ( buffer_[offset_ + 1] == '+' || buffer_[offset_ + 1] == '-' )
          offset_++;
        break;
      }
      case ' ':
      case '\n':
      case '\t':
      case '\r':
      case ',':
      case ']':
      case '}':
        goto done;
      default:
        return -1;
    }
    offset_++;
  }
done:
  if ( buffer_[offset_ - 1] >= '0' && buffer_[offset_ - 1] <= '9' ) {
    if ( dot || exp )
      tokens_.push_back( token_t( TOKEN::Double, { buffer_.data() + last_offset, offset_ - last_offset } ) );
    else
      tokens_.push_back( token_t( TOKEN::Integer, { buffer_.data() + last_offset, offset_ - last_offset } ) );
    return 0;
  }
  LEAVE_MSG( "bad number format" );
  return -1;
}

lexer::STATE lexer::Next()
{

  state = ParseWhiteSpace();
  if ( state == STATE::END )
    return state;

  // some compilers have a extension, but some not
  // case '0'...'9'
  // case 'a'...'z'
  switch ( CharType( buffer_[offset_] ) ) {
    case CHARTYPE::CHAR: {
      constexpr auto helper { []( const string_view& target ) {
        return [&target, i { 0u }]( char ch ) mutable {
          if ( i < target.size() )
            return ch == target[i++];
          return !( CharType( ch ) == CHARTYPE::OTHER );
        };
      } };
      switch ( buffer_[offset_] ) {
        case 'n': {
          constexpr static string_view sv { "null" };
          EXCEPT_ZERO( ParseNext, TOKEN::Null, helper( sv ), sv.size() );
          break;
        }
        case 't': {
          constexpr static string_view sv { "true" };
          EXCEPT_ZERO( ParseNext, TOKEN::True, helper( sv ), sv.size() );
          break;
        }
        case 'f': {
          constexpr static string_view sv { "false" };
          EXCEPT_ZERO( ParseNext, TOKEN::False, helper( sv ), sv.size() );
          break;
        }
        default: {
          // LEAVE_MSG("bad json format");
          LEAVE_MSG( std::format( "offset {}, buffer[offset] {}\n", offset_, buffer_.substr( offset_ - 3, 6 ) ) )
          throw std::runtime_error( "bad json format" );
        }
      }
      break;
    }
    case CHARTYPE::NUM: {
      if ( ParseNumber() )
        return STATE::ERROR;
      break;
    }
    case CHARTYPE::OTHER: {
      switch ( buffer_[offset_] ) {
        case '-': {
          if ( ParseNumber() )
            return STATE::ERROR;
          break;
        }
        case ',': {
          forSingelWord( TOKEN::Comma );
          break;
        }
        case ':': {
          forSingelWord( TOKEN::Colon );
          break;
        }
        case '"': {
          offset_++;
          EXCEPT_ZERO( ParseNext, TOKEN::String, []( char ch ) { return ch != '"'; }, 0 );
          offset_++;
          break;
        }
        case '[': {
          forSingelWord( TOKEN::Oparan );
          break;
        }
        case ']': {
          forSingelWord( TOKEN::Cparan );
          break;
        }
        case '{': {
          forSingelWord( TOKEN::Obrac );
          break;
        }
        case '}': {
          forSingelWord( TOKEN::Cbrac );
          break;
        }
        default: {
          LEAVE_MSG( "bad json format" );
          // throw std::runtime_error("bad json format");
          break;
        }
      }
      break;
    }
  }
  return STATE::NORMAL;
}

i32 lexer::Parse()
{
  STATE state;
  while ( true ) {
    state = this->Next();
    if ( STATE::ERROR == state ) {
      LEAVE_NOTHING;
      return -1;
    } else if ( STATE::END == state )
      break;
  }
  return 0;
}

const std::vector<token_t>& lexer::getTokens()
{
  return tokens_;
}

}; // namespace json
}; // namespace vastina