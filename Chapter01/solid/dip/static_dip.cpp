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

using MyProject = Project<FrontEndDeveloper, BackEndDeveloper>;

int main() {
  auto alice = FrontEndDeveloper{};
  auto bob = BackEndDeveloper{};
  auto new_project = MyProject{{alice, bob}};
  new_project.deliver();
}
