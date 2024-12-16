#include <cstdint>
#include <deque>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

namespace vas {

struct bf_memroy_block_t {
  std::string block;
  std::size_t pos; // pos of pointer

  bf_memroy_block_t() : block(4096, 0), pos{0u} {}
  bool isFirstPos() { return pos == 0; }
  bool isLastPos() { return pos >= block.size() - 1; }
  char &value() { return block[pos]; }
};

class bf_memroy_pool_t {
  std::deque<bf_memroy_block_t> pool{};
  std::size_t currentPos{};

public:
  bf_memroy_pool_t() { pool.push_back(bf_memroy_block_t()); }
  bf_memroy_pool_t(std::size_t size) : pool(size) {}

  void clear() {
    pool.clear();
    pool.push_back(bf_memroy_block_t());
  }

private:
  auto &current() { return pool[currentPos]; }

public:
  char &value() { return current().value(); }
  void moveRight() {
    if (current().isLastPos()) {
      currentPos++;
      if (currentPos >= pool.size()) {
        pool.push_back(bf_memroy_block_t());
      }
    } else {
      current().pos++;
    }
  }
  void moveLeft() {
    if (current().isFirstPos()) {
      if (currentPos == 0) {
        pool.push_front(bf_memroy_block_t());
      } else {
        currentPos--;
      }
      current().pos = current().block.size() - 1;
    } else {
      current().pos--;
    }
  }
  void gain() { value()++; }
  void reduce() { value()--; }
  void Input() { value() = std::getchar(); }
  void Output() { std::putchar(value()); }
};

static constexpr char valide_tokens[] {'>', '<', '+', '-',
                                             '.', ',', '[', ']'};
static constexpr char space_tokens[] {' ', '\n', '\r', '\a'};

struct bf_parser_t {
  std::string code{};
  bool valide{true};

  bf_parser_t() = default;
  void readInput(const std::string &text) {
    valide = true;
    code.clear();

    std::size_t leftBraceNum{0u};
    std::size_t rightBraceNum{0u};
    for (auto ch : text) {
      if (std::ranges::find(std::begin(valide_tokens), std::end(valide_tokens), ch) !=
          std::end(valide_tokens)) {
        if (ch == '[') {
          leftBraceNum++;
        } else if (ch == ']') {
          if (leftBraceNum <= rightBraceNum) {
            valide = false;
            break;
          }
          rightBraceNum++;
        }
        code.push_back(ch);
      } else if (std::ranges::find(std::begin(space_tokens), std::end(space_tokens),
                                   ch) == std::end(space_tokens)) {
        valide = false;
        break;
      }
    }
    if (rightBraceNum != leftBraceNum || text.size() == 0)
      valide = false;
  }
};

struct bf_executer_t {
  bf_memroy_pool_t pool{};
  bf_parser_t parser{};

  bf_executer_t() = default;
  void executeOne(char ch) {
    switch (ch) {
    case '>':
      pool.moveRight();
      break;
    case '<':
      pool.moveLeft();
      break;
    case '+':
      pool.gain();
      break;
    case '-':
      pool.reduce();
      break;
    case '.':
      pool.Output();
      break;
    case ',':
      pool.Input();
      break;
    default:
      break;
    }
  }
  void execute() {
    if (not parser.valide) {
      std::cout << "code not valide\n";
      return;
    }
    pool.clear();

    std::stack<std::pair<std::size_t, std::size_t>> braces;
    std::size_t codepos{0u};
    const auto gotoNext{[this, &codepos] {
      std::size_t right{0u};
      std::size_t left{1u};

      codepos++; // skip current [
      while (right < left) {
        if (parser.code[codepos] == '[')
          left++;
        else if (parser.code[codepos] == ']')
          right++;
        codepos++;
      }
    }};
    while (true) {
      char ch = parser.code[codepos];
      switch (ch) {
      case '[':
        if (not pool.value()) {
          if (braces.empty()) {
            gotoNext();
          } else if (braces.top().first != codepos) {
            gotoNext();
          } else if (braces.top().first == codepos) {
            if (braces.top().second == UINT64_MAX) {
              gotoNext();
            } else
              codepos = braces.top().second + 1;
            braces.pop();
          }
        } else {
          if (braces.empty()) {
            braces.push({codepos, UINT64_MAX});
          } else if (braces.top().first != codepos) {
            braces.push({codepos, UINT64_MAX});
          }
          codepos++;
        }
        break;
      case ']':
        braces.top().second = codepos;
        codepos = braces.top().first;
        break;
      default:
        executeOne(ch);
        codepos++;
      }
      if (codepos >= parser.code.size())
        break;
    }
  }
};

} // namespace vas

int main() {
  vas::bf_executer_t e{};
  {
    e.parser.readInput("");
    e.execute();
  }
  {
    e.parser.readInput("[]]");
    e.execute();
  }
  {
    e.parser.readInput("+[[[]]]<\n-a");
    e.execute();
  }
  {
    e.parser.readInput("hello world");
    e.execute();
  }
  {
    e.parser.readInput("[sdfef]]]][[...]]");
    e.execute();
  }
  {
    e.parser.readInput(
        "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.");
    e.execute();

    std::putchar('\n');
  }
  {
    e.parser.readInput("+++++"
                       "["
                       ">+++++++++++++"
                       "<-"
                       "]"
                       ">.");
    e.execute();

    std::putchar('\n');
  }
  {
    e.parser.readInput("++++++++++[>+++++++>++++++++++>+++>+<<<<-]"
                       ">++.>+.+++++++..+++.>++.<<+++++++++++++++."
                       ">.+++.------.--------.>+.>.");
    e.execute();
  }
  {
    e.parser.readInput("+-[[[]]]");
    e.execute();
  }
  {
    e.parser.readInput(",.,.");
    e.execute();
  }

  return 0;
}