#include <memory>
#include <utility>
#include <vector>

class Developer {
  virtual ~Developer() = default;
  virtual void develop() = 0;
};

class FrontEndDeveloper : public Developer {
public:
  void develop() override { developFrontEnd(); }

private:
  void developFrontEnd();
};

class BackEndDeveloper : public Developer {
public:
  void develop() override { developBackEnd(); }

private:
  void developBackEnd();
};

class Project {
public:
  using Developers = std::vector<std::unique_ptr<Developer>>;

  Project(Developers developers) : developers_{std::move(developers)} {}

  void deliver() {
    for (auto &developer : developers_) {
      developer->develop();
    }
  }

private:
  Developers developers_;
};
