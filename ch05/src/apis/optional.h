#pragma once

#include <optional>

// bad optional function parameters
void calculate_v1(int param) {}  // if param equals -1 it means "no value"
void calculate_v2(int param = -1) {}

// good optional function parameter
void calculate_v3(std::optional<int> param) {}

// bad optional return values
int try_parse_v1(std::string_view maybe_number) { return {}; }
bool try_parse_v2(std::string_view maybe_number, int &parsed_number) {
  return {};
}
int *try_parse_v3(std::string_view maybe_number) { return {}; }

// good optional return value
std::optional<int> try_parse_v4(std::string_view maybe_number) { return {}; }

using PhoneNumber = long long;

// good optional fields
struct UserProfile {
  std::string nickname;
  std::optional<std::string> full_name;
  std::optional<std::string> address;
  std::optional<PhoneNumber> phone;
};
