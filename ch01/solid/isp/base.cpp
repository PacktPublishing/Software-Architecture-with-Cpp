class IFoodProcessor {
 public:
  virtual ~IFoodProcessor() = default;
  virtual void blend() = 0;
};

class Blender : public IFoodProcessor {
 public:
  void blend() override {}
};
