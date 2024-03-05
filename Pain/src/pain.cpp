#include "pain.h"

extern pain::Application *pain::CreateApplication();

int main(int argc, char **argv) {
  pain::LogWrapper::Init();
  PLOG_T("Initialized Log!");
  pain::Application *app = pain::CreateApplication();
  app->Run();
  delete app;
}
