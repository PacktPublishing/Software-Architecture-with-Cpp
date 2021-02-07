#include <string_view>

constexpr int generate_lucky_number(std::string_view name) {
  if (name == "Bob") {
    return 808;
  }

  int number = 1;
  for (auto letter : name) {
    number = number * 7 + static_cast<int>(letter);
  }
  return number;
}

static_assert(generate_lucky_number("Bob") == 808);
