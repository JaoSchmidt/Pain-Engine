#pragma once

#include "Core.h"

namespace pain {

class Application {
public:
  Application();
  virtual ~Application();

  static int test();
  void Run();
};
// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
