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

constexpr auto MAX_HAYSTACK_SIZE = std::size_t{10'000'000};
constexpr auto NEEDLE = 2137;

void search_in_sorted_vector(benchmark::State &state, auto finder) {
  auto haystack = make_sorted_vector<int>(MAX_HAYSTACK_SIZE);
  for (auto _ : state) {
    benchmark::DoNotOptimize(finder(haystack, NEEDLE));
  }
}

BENCHMARK_CAPTURE(search_in_sorted_vector, binary, lower_bound);
BENCHMARK_CAPTURE(search_in_sorted_vector, linear, find);

}  // namespace

BENCHMARK_MAIN();
