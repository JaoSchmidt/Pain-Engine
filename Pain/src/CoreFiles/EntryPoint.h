#pragma once

#include "CoreFiles/Application.h"

extern pain::Application *pain::CreateApplication();

int main(int argc, char **argv) {
  auto app = pain::CreateApplication();
  app->Run();
  delete app;
}
