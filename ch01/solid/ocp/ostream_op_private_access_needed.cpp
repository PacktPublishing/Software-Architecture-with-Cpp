#include <ostream>

template <typename T, typename U>
class MyPair {
 public:
  T &firstMember();
  U &secondMember();

 private:
  friend std::ostream &operator<<(std::ostream &stream, const MyPair &mp);

  T first_;
  U second_;
  int secretThirdMember_;
};

std::ostream &operator<<(std::ostream &stream, const MyPair &mp) {
  stream << mp.first_;
  stream << mp.second_;
  stream << mp.secretThirdMember_;
  return stream;
}
