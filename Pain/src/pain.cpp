#include "pain.h"
#include "CoreFiles/LogWrapper.h"

extern pain::Application *pain::CreateApplication();

int main(int argc, char **argv) {
  pain::LogWrapper::Init();
  LOG_WARN_C("Initialized Log!");
  int a = 5;
  LOG_INFO("Hello! Var={0}", a);
  pain::Application *app = pain::CreateApplication();
  app->Run();
  delete app;
}
