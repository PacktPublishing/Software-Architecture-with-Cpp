class Rectangle {
 public:
  virtual ~Rectangle() = default;
  virtual double area() { return width_ * height_; }
  virtual void setWidth(double width) { width_ = width; }
  virtual void setHeight(double height) { height_ = height; }

 private:
  double width_;
  double height_;
};

class Square : public Rectangle {
 public:
  double area() override { return Rectangle::area(); }
  void setWidth(double width) override {
    Rectangle::setWidth(width);
    Rectangle::setHeight(width);
  }
  void setHeight(double height) override { setWidth(height); }
};
