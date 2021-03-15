#include <algorithm>
#include <string_view>

int count_dots(std::string_view str) {
  return std::count(std::begin(str), std::end(str), '.');
}
