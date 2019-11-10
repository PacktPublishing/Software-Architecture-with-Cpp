class Rectangle {
public:
  virtual ~Rectangle() = default;
  virtual double area() { return width_ * height_; }
  virtual void setWidth(double width) { width_ = width; }
  virtual void setHeight(double height) { height_ = height_; }

private:
  double width_;
  double height_;
};

class Square : public Rectangle {
public:
  double area() override;
  void setWidth(double width) override;
  void setHeight(double height) override;
};
