namespace factory_method {

class Pixel {
 public:
  static Pixel fromRgba(char r, char g, char b, char a) {
    return Pixel{r, g, b, a};
  }
  static Pixel fromBgra(char b, char g, char r, char a) {
    return Pixel{r, g, b, a};
  }

  // other members

 private:
  Pixel(char r, char g, char b, char a) : r_(r), g_(g), b_(b), a_(a) {}
  char r_, g_, b_, a_;
};

}  // namespace factory_method

namespace factory_function {

struct Pixel {
  char r, g, b, a;

 private:
  Pixel(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) {}
  friend Pixel makePixelFromRgba(char r, char g, char b, char a);
  friend Pixel makePixelFromBgra(char b, char g, char r, char a);
};

Pixel makePixelFromRgba(char r, char g, char b, char a) {
  return Pixel{r, g, b, a};
}

Pixel makePixelFromBgra(char b, char g, char r, char a) {
  return Pixel{r, g, b, a};
}

}  // namespace factory_function

int main() {
  auto black_pixel = factory_method::Pixel::fromRgba(0, 0, 0, 0);
  auto white_pixel = factory_function::makePixelFromRgba(255, 255, 255, 0);
}
