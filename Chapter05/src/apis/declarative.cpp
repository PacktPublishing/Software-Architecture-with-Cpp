#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

using namespace std::literals;

std::optional<std::size_t> imperative_aka_bad() {
  auto temperatures = std::vector<double>{-3., 2., 0., 8., -10., -7.};
  // ...
  for (std::size_t i = 0; i < temperatures.size() - 1; ++i) {
    for (std::size_t j = i + 1; j < temperatures.size(); ++j) {
      if (std::abs(temperatures[i] - temperatures[j]) > 5)
        return std::optional{i};
    }
  }
  return std::nullopt;
}

std::optional<std::size_t> legacy_declarative() {
  auto temperatures = std::vector<double>{-3., 2., 0., 8., -10., -7.};
  // ...
  auto it = std::ranges::adjacent_find(
      temperatures,
      [](double first, double second) { return std::abs(first - second) > 5; });
  if (it != std::end(temperatures)) {
    return std::optional{std::distance(std::begin(temperatures), it)};
  }
  return std::nullopt;
}

void modern_declarative() {
  using namespace std::ranges;

  auto is_even = [](auto x) { return x % 2 == 0; };
  auto to_string = [](auto x) { return std::to_string(x); };
  auto my_range = views::iota(1) | views::filter(is_even) | views::take(2) |
                  views::reverse | views::transform(to_string);
  std::cout << std::accumulate(begin(my_range), end(my_range), ""s) << '\n';
}
