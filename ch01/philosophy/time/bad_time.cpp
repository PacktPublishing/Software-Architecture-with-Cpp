struct Duration {
  int millis_;
};

Duration d;
d.millis_ = 100;

auto timeout = 1;     // second
d.millis_ = timeout;  // ouch, we meant 1000 millis but assigned just 1
