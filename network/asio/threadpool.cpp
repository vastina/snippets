#include <asio.hpp>
#include <asio/static_thread_pool.hpp>
#include <cstdlib>
#include <fcntl.h>
#include <string>

using asio::static_thread_pool;

int main() {
  std::system("mkdir -p test");
  static_thread_pool pool( 4 );
  for(int i=0; i<100; i++) {
    pool.executor().execute([i] {
      auto str {std::string("test/") + std::to_string(i) + std::string(".txt")};
      int fd = ::open(str.data(), O_CREAT | O_RDWR, 0666);
      ::write(fd, str.data(), str.size());
      ::close(fd);
    });
  }
  pool.scheduler().execute([]{});
  // pool.join();
  return 0;
}