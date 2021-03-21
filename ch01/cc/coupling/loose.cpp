#include <memory>
#include <utility>
#include <variant>
#include <vector>

class FrontEndDeveloper {
 public:
  void develop() { developFrontEnd(); }

 private:
  void developFrontEnd() {}
};

class BackEndDeveloper {
 public:
  void develop() { developBackEnd(); }

 private:
  void developBackEnd() {}
};

class MiddlewareDeveloper {
 public:
  void develop() { developMiddleware(); }

 private:
  void developMiddleware() {}
};

template <typename... Devs>
class Project {
 public:
  using Developers = std::vector<std::variant<Devs...>>;

  explicit Project(Developers developers)
      : developers_{std::move(developers)} {}

  void deliver() {
    for (auto &developer : developers_) {
      std::visit([](auto &dev) { dev.develop(); }, developer);
    }
  }

 private:
  Developers developers_;
};

using MyProject =
    Project<FrontEndDeveloper, MiddlewareDeveloper, BackEndDeveloper>;

int main() {
  auto alice = FrontEndDeveloper{};
  auto bob = BackEndDeveloper{};
  auto charlie = MiddlewareDeveloper{};
  auto new_project = MyProject{{alice, charlie, bob}};
  new_project.deliver();
}

// TEST(Project, UsesAllDevelopers) {
// // implementable by inserting mocks in place of developers
// }
