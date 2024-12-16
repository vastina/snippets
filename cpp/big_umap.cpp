#include <unordered_map>
#include <string>

static constexpr std::pair<int, const char*> some_big_compileTime_data[]
{ {1, "1"}, {2, "1"}, /*...*/ };
static const std::unordered_map<int, std::string> big_compileTime_data
(std::begin(some_big_compileTime_data), std::end(some_big_compileTime_data));

// https://www.zhihu.com/question/1353595546/answer/10780622099
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2752r3.html