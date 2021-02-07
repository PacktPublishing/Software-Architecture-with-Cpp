#include <algorithm>
#include <cassert>
#include <utility>

template <typename T>
class Array {
 public:
  Array(T *array, int size) : array_{array}, size_{size} {}

  Array(const Array &other) : array_{new T[other.size_]}, size_{other.size_} {
    std::copy_n(other.array_, size_, array_);
  }

  Array(Array &&other) noexcept
      : array_{std::exchange(other.array_, nullptr)},
        size_{std::exchange(other.size_, 0)} {}

  Array &operator=(Array other) noexcept {
    swap(*this, other);
    return *this;
  }

  ~Array() { delete[] array_; }

  // swap functions should never throw
  friend void swap(Array &left, Array &right) noexcept {
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

int main() {
  auto my_array = make_array<int>(7);
  auto my_move_constructed_array = std::move(my_array);
  my_array = std::move(my_move_constructed_array);  // move back
  auto my_copy_constructed_array = my_array;
  my_array = my_copy_constructed_array;  // copy back
  assert(my_array.size() == 7);
}
