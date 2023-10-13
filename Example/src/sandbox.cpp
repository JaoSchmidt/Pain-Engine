#include <iostream>
#include <pain.h>

class Sandbox : public pain::Application {
public:
  Sandbox() {}

  ~Sandbox() {}
};

pain::Application *pain::CreateApplication() { return new Sandbox(); }
