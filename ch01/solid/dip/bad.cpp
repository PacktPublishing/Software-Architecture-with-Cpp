class FrontEndDeveloper {
 public:
  void developFrontEnd() {}
};

class BackEndDeveloper {
 public:
  void developBackEnd() {}
};

class Project {
 public:
  void deliver() {
    fed_.developFrontEnd();
    bed_.developBackEnd();
  }

 private:
  FrontEndDeveloper fed_;
  BackEndDeveloper bed_;
};

int main() {
  auto project = Project{};
  project.deliver();
}
