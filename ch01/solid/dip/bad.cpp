class FrontEndDeveloper {
public:
  void developFrontEnd();
};

class BackEndDeveloper {
public:
  void developBackEnd();
};

class Project {
public:
  void deliver() {
    fed_.developFrontEnd();
    bed_.developBackend();
  }

private:
  FrontEndDeveloper fed_;
  BackEndDeveloper bed_;
};
