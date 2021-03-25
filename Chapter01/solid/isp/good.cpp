class IBlender {
 public:
  virtual ~IBlender() = default;
  virtual void blend() = 0;
};

class ICutter {
 public:
  virtual ~ICutter() = default;
  virtual void slice() = 0;
  virtual void dice() = 0;
};

class Blender : public IBlender {
 public:
  void blend() override {}
};

class AnotherFoodProcessor : public IBlender, public ICutter {
 public:
  void blend() override {}
  void slice() override {}
  void dice() override {}
};
