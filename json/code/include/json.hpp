#ifndef _VAS_JSON_H_
#define _VAS_JSON_H_

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <cstdint>

namespace vastina {

using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::string_view;

namespace json {

enum class j_type
{
  object,
  array,
  string,
  number,
  boolean,
  null
};

enum j_Parse
{
  standard,
  comment
};

class json;

using array = std::vector<json>;
using object = std::unordered_map<string_view, json>;

class j_value
{};

template<j_type type, typename ty>
class value_wrapper
{};

class j_integer final : public value_wrapper<j_type::number, i64>
{};

class j_double final : public value_wrapper<j_type::number, double>
{};

class j_obj final : public value_wrapper<j_type::object, object>
{};

class j_array final : public value_wrapper<j_type::array, array>
{};

class j_str final : public value_wrapper<j_type::string, string>
{};

class j_boolean final : public value_wrapper<j_type::boolean, bool>
{};

class j_NULL final : public value_wrapper<j_type::null, std::nullptr_t>
{};

struct constant
{
  constexpr static j_boolean j_true {};
  constexpr static j_boolean j_false {};
  constexpr static j_NULL j_null {};
};

class json final
{

private:
  shared_ptr<j_value> _M_ptr;
};

}

}; //namespace vastina

#endif