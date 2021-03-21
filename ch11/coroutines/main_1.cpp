#include <fmt/format.h>
#include <fmt/ostream.h>

#include <chrono>
#include <cppcoro/async_mutex.hpp>
#include <cppcoro/schedule_on.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/when_all.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

inline constexpr auto WORK_ITEMS = 5;

cppcoro::task<> fill_number(int i, std::vector<int> &ints,
                            cppcoro::async_mutex &mutex) {
  fmt::print("Thread {}: producing {}\n", std::this_thread::get_id(), i);
  std::this_thread::sleep_for(
      std::chrono::milliseconds((WORK_ITEMS - i) * 200));

  {
    auto lock = co_await mutex.scoped_lock_async();  // remember to co_await!
    ints.emplace_back(i);
  }

  fmt::print("Thread {}: produced {}\n", std::this_thread::get_id(), i);
  co_return;
}

cppcoro::task<std::vector<int>> do_routine_work(
    cppcoro::static_thread_pool &thread_pool) {
  auto mutex = cppcoro::async_mutex{};
  auto ints = std::vector<int>{};
  ints.reserve(WORK_ITEMS);

  fmt::print("Thread {}: passing execution to the pool\n",
             std::this_thread::get_id());

  co_await thread_pool.schedule();
  fmt::print("Thread {}: running first pooled job\n",
             std::this_thread::get_id());

  std::vector<cppcoro::task<>> tasks;
  for (int i = 0; i < WORK_ITEMS; ++i) {
    tasks.emplace_back(
        cppcoro::schedule_on(thread_pool, fill_number(i, ints, mutex)));
  }

  co_await cppcoro::when_all_ready(std::move(tasks));

  co_return ints;
}

int main() {
  auto thread_pool = cppcoro::static_thread_pool{3};

  fmt::print("Thread {}: preparing work\n", std::this_thread::get_id());
  auto work = do_routine_work(thread_pool);

  fmt::print("Thread {}: starting work\n", std::this_thread::get_id());
  const auto ints = cppcoro::sync_wait(work);

  fmt::print("Thread {}: work done. Produced ints are: ",
             std::this_thread::get_id());
  for (auto i : ints) {
    fmt::print("{}, ", i);
  }
  fmt::print("\n");
}
