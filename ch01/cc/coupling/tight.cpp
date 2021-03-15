class FrontEndDeveloper {
 public:
  void developFrontEnd() {}
};

class BackEndDeveloper {
 public:
  void developBackEnd() {}
};

class MiddlewareDeveloper {
 public:
  void developMiddleware() {}
};

class Project {
 public:
  void deliver() {
    fed_.developFrontEnd();
    med_.developMiddleware();
    bed_.developBackEnd();
  }

 private:
  FrontEndDeveloper fed_;
  MiddlewareDeveloper med_;
  BackEndDeveloper bed_;
};

int main() {
  Project p{};
  p.deliver();
}

// TEST(Project, UsesAllDevelopers) {
// // impossible to implement
// }
