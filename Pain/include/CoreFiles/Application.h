#pragma once

#include "Core.h"

namespace pain {

class EXPORT Application {
public:
  Application();
  virtual ~Application();

  void Run();
};
// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
