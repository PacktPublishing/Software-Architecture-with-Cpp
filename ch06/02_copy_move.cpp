struct NonCopyable {
  NonCopyable() = default;
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  // NOTE: also non-movable
};

class MyType : NonCopyable {};

struct MyTypeV2 {
  MyTypeV2() = default;
  MyTypeV2(const MyTypeV2 &) = delete;
  MyTypeV2 &operator=(const MyTypeV2 &) = delete;
  MyTypeV2(MyTypeV2 &&) = delete;
  MyTypeV2 &operator=(MyTypeV2 &&) = delete;
};

int main() {
  // in C++17+ this works despite non-copyablility due to mandatory copy elision
  auto my_object = MyType{};
  auto my_better_object = MyTypeV2{};
}
