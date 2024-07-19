#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include "json.hpp"

namespace vastina {

namespace json {

struct ParserConfig {
  ParserConfig(...) = delete;

  //if this is true, {last_key:last_value,} is allowed
  static bool last_comma ;
  //if this is true, use Scientific notation is allowed, 1.14514*10^1919810
  static bool science_num ;
  /*
  if this is true,
  "114""514" is allowed
  "1919"
  "810" is also allowed
  */
  static bool more_str;
  // I don't want to impl this
  static bool num_as_key;
  
  static void UseDefault();
};


}

}

#endif