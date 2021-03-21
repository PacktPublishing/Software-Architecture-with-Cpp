#include <stdexcept>

class IFoodProcessor {
 public:
  virtual ~IFoodProcessor() = default;
  virtual void blend() = 0;
  virtual void slice() = 0;
  virtual void dice() = 0;
};

class AnotherFoodProcessor : public IFoodProcessor {
 public:
  void blend() override {}
  void slice() override {}
  void dice() override {}
};

class Blender : public IFoodProcessor {
 public:
  void blend() override {}
  void slice() override { throw std::logic_error{"I can't do that"}; }
  void dice() override { throw std::logic_error{"Oh no!"}; }
};
