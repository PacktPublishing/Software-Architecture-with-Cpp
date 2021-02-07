#include <memory>

class PotentiallyMisleading {
 public:
  PotentiallyMisleading() = default;
  PotentiallyMisleading(const PotentiallyMisleading &) = default;
  PotentiallyMisleading &operator=(const PotentiallyMisleading &) = default;
  PotentiallyMisleading(PotentiallyMisleading &&) = default;
  PotentiallyMisleading &operator=(PotentiallyMisleading &&) = default;
  ~PotentiallyMisleading() = default;

 private:
  std::unique_ptr<int> int_;
};

class RuleOfZero {
  std::unique_ptr<int> int_;
};

int main() {
  auto pm = PotentiallyMisleading{};
  auto pm2 = std::move(pm);
  // below line won't compile, but that's not always obvious
  // auto pm2_copy = pm2;

  auto roz = RuleOfZero{};
  auto roz2 = std::move(roz);
  // below line also won't compile, but that's not surprising
  // auto roz2_copy = roz2;
}
