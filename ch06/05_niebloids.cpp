#include <algorithm>
#include <deque>
#include <gsl/pointers>
#include <iostream>
#include <ostream>
#include <ranges>

namespace my_ranges {
namespace detail {
struct contains_fn final {
  template <std::input_iterator It, std::sentinel_for<It> Sent, typename T,
            typename Proj = std::identity>
  requires std::indirect_binary_predicate<
      std::ranges::equal_to, std::projected<It, Proj>, const T *> constexpr bool
  operator()(It first, Sent last, const T &value, Proj projection = {}) const {
    while (first != last && std::invoke(projection, *first) != value) ++first;
    return first != last;
  }

  template <std::ranges::input_range Range, typename T,
            typename Proj = std::identity>
  requires std::indirect_binary_predicate<
      std::ranges::equal_to,
      std::projected<std::ranges::iterator_t<Range>, Proj>,
      const T *> constexpr bool
  operator()(Range &&range, const T &value, Proj projection = {}) const {
    return (*this)(std::ranges::begin(range), std::ranges::end(range), value,
                   std::move(projection));
  }
};
}  // namespace detail

inline constexpr detail::contains_fn contains{};
}  // namespace my_ranges

int main() {
  constexpr auto ints =
      std::ranges::views::iota(0) | std::ranges::views::take(5);

  return my_ranges::contains(ints, 42);
}
