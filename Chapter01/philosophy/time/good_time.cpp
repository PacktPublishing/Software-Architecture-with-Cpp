#include <chrono>

using namespace std::literals::chrono_literals;

struct Duration {
  std::chrono::milliseconds millis_;
};

void example() {
  auto d = Duration{};
  // d.millis_ = 100; // compilation error, as 100 could mean anything
  d.millis_ = 100ms;  // okay

  auto timeout = 1s;  // or std::chrono::seconds(1);
  d.millis_ =
      timeout;  // okay, seconds will be converted automatically to milliseconds
}
