#include <algorithm>
#include <iostream>
#include <string_view>

struct NullPrintingPolicy {
  template <typename... Args>
  void operator()(Args...) {}
};

struct CoutPrintingPolicy {
  void operator()(std::string_view text) { std::cout << text << std::endl; }
};

template <typename T, typename DebugPrintingPolicy = NullPrintingPolicy>
class Array {
 public:
  Array(T *array, int size) : array_{array}, size_{size} {
    DebugPrintingPolicy{}("constructor");
  }

  Array(const Array &other) : array_{new T[other.size_]}, size_{other.size_} {
    DebugPrintingPolicy{}("copy constructor");
    std::copy_n(other.array_, size_, array_);
  }

  Array(Array &&other) noexcept
      : array_{std::exchange(other.array_, nullptr)},
        size_{std::exchange(other.size_, 0)} {
    DebugPrintingPolicy{}("move constructor");
  }

  Array &operator=(Array other) noexcept {
    DebugPrintingPolicy{}("assignment operator");
    swap(*this, other);
    return *this;
  }

  ~Array() {
    DebugPrintingPolicy{}("destructor");
    delete[] array_;
  }

  // swap functions should never throw
  friend void swap(Array &left, Array &right) noexcept {
    DebugPrintingPolicy{}("swap");
    using std::swap;
    swap(left.array_, right.array_);
    swap(left.size_, right.size_);
  }

  T &operator[](int index) { return array_[index]; }
  int size() const { return size_; }

 private:
  T *array_;
  int size_;
};

template <typename T>
Array<T> make_array(int size) {
  return Array(new T[size], size);
}

template <typename T>
Array<T, CoutPrintingPolicy> make_verbose_array(int size) {
  return Array<T, CoutPrintingPolicy>(new T[size], size);
}

int main() {
  {
    auto my_array = make_array<int>(7);
    auto my_array_copy = my_array;
    my_array = my_array_copy;
  }
  {
    auto my_verbose_array = make_verbose_array<int>(13);
    auto my_verbose_array_copy = my_verbose_array;
    my_verbose_array = my_verbose_array_copy;
  }
}
