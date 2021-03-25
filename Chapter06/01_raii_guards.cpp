#include <chrono>
#include <gsl/gsl_util>
#include <iostream>
#include <ostream>

using namespace std::chrono;

void self_measuring_function() {
  auto timestamp_begin = high_resolution_clock::now();

  auto cleanup = gsl::finally([timestamp_begin] {
    auto timestamp_end = high_resolution_clock::now();
    std::cout
        << "Execution took: "
        << duration_cast<microseconds>(timestamp_end - timestamp_begin).count()
        << " us";
  });

  // perform work
  // throw std::runtime_error{"Unexpected fault"};
}

int main() try { self_measuring_function(); } catch (...) {
}
