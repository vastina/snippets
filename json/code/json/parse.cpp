#include "parse.hpp"

namespace vastina {

namespace json {

void ParserConfig::UseDefault(){
  last_comma = false;
  science_num = false;
  more_str= false;
  num_as_key = false;
}

}

}