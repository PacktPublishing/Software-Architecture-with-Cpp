#include <ostream>

template <typename T, typename U>
class MyPair {
 public:
  T &firstMember() { return first_; }
  const T &firstMember() const { return first_; }
  U &secondMember() { return second_; }
  const U &secondMember() const { return second_; }

 private:
  friend std::ostream &operator<<(std::ostream &stream, const MyPair &mp);

  T first_;
  U second_;
  int secretThirdMember_;
};

std::ostream &operator<<(std::ostream &stream, const MyPair<int, int> &mp) {
  stream << mp.first_;
  stream << mp.second_;
  stream << mp.secretThirdMember_;
  return stream;
}
