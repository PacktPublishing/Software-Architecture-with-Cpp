#include <iostream>
#include <ostream>

template <typename T, typename U>
class MyPair {
 public:
  T &firstMember() { return first_; }
  const T &firstMember() const { return first_; }
  U &secondMember() { return second_; }
  const U &secondMember() const { return second_; }

 private:
  template <typename X, typename Y>
  friend std::ostream &operator<<(std::ostream &stream, const MyPair<X, Y> &mp);

  T first_;
  U second_;
  int secretThirdMember_;
};

template <typename T, typename U>
std::ostream &operator<<(std::ostream &stream, const MyPair<T, U> &mp) {
  stream << mp.first_ << ", ";
  stream << mp.second_ << ", ";
  stream << mp.secretThirdMember_;
  return stream;
}

int main() {
  auto mp = MyPair<int, int>{};
  std::cout << mp << '\n';
}
