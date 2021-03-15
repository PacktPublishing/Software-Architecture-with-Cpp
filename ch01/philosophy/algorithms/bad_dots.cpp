#include <cstddef>

int count_dots(const char *str, std::size_t len) {
  int count = 0;
  for (std::size_t i = 0; i < len; ++i) {
    if (str[i] == '.') count++;
  }
  return count;
}
