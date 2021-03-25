#include <benchmark/benchmark.h>

#include <ranges>
#include <vector>

using namespace std::ranges;

namespace {

template <typename T>
auto make_sorted_vector(std::size_t size) {
  auto sorted = std::vector<T>{};
  sorted.reserve(size);

  auto sorted_view = views::iota(T{0}) | views::take(size);
  std::ranges::copy(sorted_view, std::back_inserter(sorted));
  return sorted;
}

constexpr auto MIN_HAYSTACK_SIZE = std::size_t{1'000};
constexpr auto MAX_HAYSTACK_SIZE = std::size_t{10'000'000};

void search_in_sorted_vector(benchmark::State &state, auto finder) {
  const auto haystack = make_sorted_vector<int>(state.range(0));
  const auto needle = 2137;
  for (auto _ : state) {
    benchmark::DoNotOptimize(finder(haystack, needle));
  }
}

BENCHMARK_CAPTURE(search_in_sorted_vector, binary, lower_bound)
    ->RangeMultiplier(10)
    ->Range(MIN_HAYSTACK_SIZE, MAX_HAYSTACK_SIZE);
BENCHMARK_CAPTURE(search_in_sorted_vector, linear, find)
    ->RangeMultiplier(10)
    ->Range(MIN_HAYSTACK_SIZE, MAX_HAYSTACK_SIZE);

}  // namespace

BENCHMARK_MAIN();
