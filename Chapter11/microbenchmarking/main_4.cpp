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
  const auto needle = state.range(0);
  const auto haystack = make_sorted_vector<int>(state.range(1));
  for (auto _ : state) {
    benchmark::DoNotOptimize(finder(haystack, needle));
  }
}

void generate_sizes(benchmark::internal::Benchmark *b) {
  for (long haystack = MIN_HAYSTACK_SIZE; haystack <= MAX_HAYSTACK_SIZE;
       haystack *= 100) {
    for (auto needle :
         {haystack / 8, haystack / 2, haystack - 1, haystack + 1}) {
      b->Args({needle, haystack});
    }
  }
}

BENCHMARK_CAPTURE(search_in_sorted_vector, binary, lower_bound)
    ->Apply(generate_sizes);
BENCHMARK_CAPTURE(search_in_sorted_vector, linear, find)->Apply(generate_sizes);

}  // namespace

BENCHMARK_MAIN();
