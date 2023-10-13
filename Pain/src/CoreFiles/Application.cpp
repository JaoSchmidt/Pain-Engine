#include "Application.h"
#include <iostream>
#include <ostream>

namespace pain {

Application::Application() {}

Application::~Application() {}

int Application::test() { return 28; }

void Application::Run() {
  while (true)
    std::cout << "testing" << std::endl;
  ;
}

} // namespace pain
