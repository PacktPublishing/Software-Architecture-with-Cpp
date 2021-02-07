#include <array>
#include <iostream>
#include <tuple>
#include <utility>
#include <variant>

template <typename ConcreteItem>
class GlamorousItem {
 public:
  void appear_in_full_glory() {
    static_cast<ConcreteItem*>(this)->appear_in_full_glory();
  }
};

class PinkHeels : public GlamorousItem<PinkHeels> {
 public:
  void appear_in_full_glory() {
    std::cout << "Pink high heels suddenly appeared in all their beauty\n";
  }
};

class GoldenWatch : public GlamorousItem<GoldenWatch> {
 public:
  void appear_in_full_glory() {
    std::cout << "Everyone wanted to watch this watch\n";
  }
};

template <typename... Args>
using PreciousItems = std::tuple<GlamorousItem<Args>...>;

using GlamorousVariant = std::variant<PinkHeels, GoldenWatch>;

class CommonGlamorousItem {
 public:
  template <typename T>
  requires std::is_base_of_v<GlamorousItem<T>, T> explicit CommonGlamorousItem(
      T&& item)
      : item_{std::forward<T>(item)} {}

  void appear_in_full_glory() {
    std::visit(
        []<typename T>(GlamorousItem<T> item) { item.appear_in_full_glory(); },
        item_);
  }

 private:
  GlamorousVariant item_;
};

int main() {
  {
    auto glamorous_items = PreciousItems<PinkHeels, GoldenWatch>{};
    std::apply([]<typename... T>(GlamorousItem<T>... items) {
      (items.appear_in_full_glory(), ...);
    },
               glamorous_items);
  }
  std::cout << "---\n";
  {
    auto glamorous_items = std::array{GlamorousVariant{PinkHeels{}},
                                      GlamorousVariant{GoldenWatch{}}};
    for (auto& elem : glamorous_items) {
      std::visit([]<typename T>(GlamorousItem<T> item) {
        item.appear_in_full_glory();
      },
                 elem);
    }
  }
  std::cout << "---\n";
  {
    auto glamorous_items = std::array{CommonGlamorousItem{PinkHeels{}},
                                      CommonGlamorousItem{GoldenWatch{}}};
    for (auto& elem : glamorous_items) {
      elem.appear_in_full_glory();
    }
  }
}
