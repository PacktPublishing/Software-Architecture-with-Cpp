#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory_resource>
#include <string>
#include <vector>

void using_memory_arenas() {
  auto single_threaded_pool = std::pmr::unsynchronized_pool_resource();
  std::pmr::set_default_resource(&single_threaded_pool);

  auto ints = std::vector<int, std::pmr::polymorphic_allocator<int>>(
      std::pmr::get_default_resource());
  auto also_ints = std::pmr::vector<int>{};
}

class verbose_resource : public std::pmr::memory_resource {
 public:
  explicit verbose_resource(std::pmr::memory_resource *upstream_resource)
      : upstream_resource_(upstream_resource) {}

 private:
  void *do_allocate(size_t bytes, size_t alignment) override {
    std::cout << "Allocating " << bytes << " bytes\n";
    return upstream_resource_->allocate(bytes, alignment);
  }

  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    std::cout << "Deallocating " << bytes << " bytes\n";
    upstream_resource_->deallocate(p, bytes, alignment);
  }

  [[nodiscard]] bool do_is_equal(const memory_resource &other) const
      noexcept override {
    return this == &other;
  }

  std::pmr::memory_resource *upstream_resource_;
};

void using_pool_resources_and_writing_your_own() {
  auto buffer = std::array<std::byte, 1 * 1024 * 1024>{};
  auto monotonic_resource =
      std::pmr::monotonic_buffer_resource{buffer.data(), buffer.size()};
  auto pool_options = std::pmr::pool_options{
      .max_blocks_per_chunk = 0, .largest_required_pool_block = 512};
  auto arena =
      std::pmr::unsynchronized_pool_resource{pool_options, &monotonic_resource};
  auto verbose_arena = verbose_resource(&arena);
  auto pooled_ints = std::pmr::vector<int>(&verbose_arena);
  for (int i = 0; i < 1729; ++i) {
    pooled_ints.push_back(i);
  }
}

void ensuring_no_unexpected_allocations() {
  std::pmr::set_default_resource(std::pmr::null_memory_resource());

  auto buffer = std::array<std::byte, 640 * 1024>{};  // 640K ought to be enough
                                                      // for anybody
  auto resource = std::pmr::monotonic_buffer_resource{
      buffer.data(), buffer.size(), std::pmr::null_memory_resource()};

  try {
    std::pmr::unordered_map<int, std::pmr::string>{{-1, "whoops"}};
  } catch (std::bad_alloc &) {
    std::cout << "Caught a bad alloc\n";
  }

  set_default_resource(std::pmr::new_delete_resource());
}

void winking_out() {
  auto verbose = verbose_resource(std::pmr::get_default_resource());
  auto monotonic = std::pmr::monotonic_buffer_resource(&verbose);
  std::pmr::set_default_resource(&monotonic);

  auto alloc = std::pmr::polymorphic_allocator{};
  auto *vector = alloc.new_object<std::pmr::vector<std::pmr::string>>();
  vector->push_back("first one");
  vector->emplace_back("long second one that must allocate");

  // about to wink out
}

int main() {
  using_memory_arenas();
  std::cout << "---\n";
  using_pool_resources_and_writing_your_own();
  std::cout << "---\n";
  ensuring_no_unexpected_allocations();
  std::cout << "---\n";
  winking_out();
}
