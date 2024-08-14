// Makefile can do this, but I donot want to write it

#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

// constexpr
const std::vector<std::string_view>
  types { ".c", ".h", ".cpp", ".hpp", ".cc", ".hh", ".cxx", ".ixx", ".glsl" };

const std::vector<std::string_view> notignored { "testcase", "tests" };

const std::vector<std::string_view> ignored {
  "build",
  "test",
  "dep",
  "third_party",
  "thirdparty",
  "third-part",
  "python3", // files from python-venv
  ".git",
  ".cache",
  "static",
};

bool shouldDeal( const std::string& path )
{
  if ( std::ranges::all_of( notignored, [&path]( const std::string_view& notignore ) {
         return path != notignore;
       } ) ) {
    return false;
  }
  // for ( const auto& notignore : notignored ) {
  //   if ( path == notignore ) {
  //     return true;
  //   }
  // }
  return std::ranges::all_of( ignored, [&path]( const std::string_view& ignore ) {
    return path.find( ignore ) == std::string::npos;
  } );
}

fs::file_time_type lastFormatTime {};

void doFormat( const fs::path& path )
{
  for ( const auto& entry : fs::directory_iterator( path ) ) {
    if ( entry.is_directory() ) {
      if ( shouldDeal( entry.path().filename().string() ) )
        doFormat( entry.path() );
    } else {
      const auto lastWritetime { fs::last_write_time( entry ) };
      if ( lastWritetime < lastFormatTime )
        continue;
      const auto filename { entry.path().string() };
      for ( const auto& type : types ) {
        if ( filename.ends_with( type ) ) {
          // std::cout << "Formatting: " << file << std::endl;
          std::system( ( "clang-format -i " + filename ).data() );
          break;
        }
      }
    }
  }
}

void genTimeStamp()
{
  const static char* dirname { "./test/timestamp" };
  if( not fs::exists( dirname ) ) {
    // format all
    lastFormatTime = fs::file_time_type{};
  } else {
    lastFormatTime = fs::last_write_time( dirname );
  }
  fs::remove( dirname );
  fs::create_directories( dirname );
}

int main( int argc, char* argv[] )
{
  genTimeStamp();
  fs::path p( argc > 1 ? argv[1] : "./" );
  doFormat( p );

  return 0;
}