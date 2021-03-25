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

void binary_search_in_sorted_vector(benchmark::State &state) {
  auto haystack = make_sorted_vector<int>(MAX_HAYSTACK_SIZE);
  for (auto _ : state) {
    benchmark::DoNotOptimize(lower_bound(haystack, NEEDLE));
  }
}

void linear_search_in_sorted_vector(benchmark::State &state) {
  auto haystack = make_sorted_vector<int>(MAX_HAYSTACK_SIZE);
  for (auto _ : state) {
    benchmark::DoNotOptimize(find(haystack, NEEDLE));
  }
}

BENCHMARK(linear_search_in_sorted_vector);
BENCHMARK(binary_search_in_sorted_vector);

}  // namespace

BENCHMARK_MAIN();
