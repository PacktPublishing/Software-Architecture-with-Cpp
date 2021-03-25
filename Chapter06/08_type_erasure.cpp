#include <array>
#include <iostream>
#include <memory>

class GlamorousItem {
 public:
  template <typename T>
  explicit GlamorousItem(T t)
      : item_{std::make_unique<TypeErasedItem<T>>(std::move(t))} {}

  void appear_in_full_glory() { item_->appear_in_full_glory_impl(); }

 private:
  struct TypeErasedItemBase {
    virtual ~TypeErasedItemBase() = default;
    virtual void appear_in_full_glory_impl() = 0;
  };

  template <typename T>
  class TypeErasedItem final : public TypeErasedItemBase {
   public:
    explicit TypeErasedItem(T t) : t_{std::move(t)} {}
    void appear_in_full_glory_impl() override { t_.appear_in_full_glory(); }

   private:
    T t_;
  };

  std::unique_ptr<TypeErasedItemBase> item_;
};

class PinkHeels {
 public:
  void appear_in_full_glory() {
    std::cout << "Pink high heels suddenly appeared in all their beauty\n";
  }
};

class GoldenWatch {
 public:
  void appear_in_full_glory() {
    std::cout << "Everyone wanted to watch this watch\n";
  }
};

int main() {
  auto glamorous_items =
      std::array{GlamorousItem{PinkHeels{}}, GlamorousItem{GoldenWatch{}}};
  for (auto &item : glamorous_items) {
    item.appear_in_full_glory();
  }
}
