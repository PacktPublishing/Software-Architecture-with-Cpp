#include <algorithm>
#include <array>

struct Merchant {
  int id;
};

constexpr bool has_merchant(const Merchant &selected) {
  auto merchants = std::array{Merchant{1}, Merchant{2}, Merchant{3},
                              Merchant{4}, Merchant{5}};
  return std::binary_search(merchants.begin(), merchants.end(), selected,
                            [](auto a, auto b) { return a.id < b.id; });
}

int main() { return has_merchant({999}); }
