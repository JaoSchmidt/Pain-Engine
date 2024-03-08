#include "pain.h"
#include "GL/glew.h"

extern pain::Application *pain::CreateApplication();

pain::Application *app;

int main(int argc, char **argv)
{
  pain::LogWrapper::Init();

  PLOG_T("Initialized Log!");
  app = pain::CreateApplication();

  app->run();
  delete app;
}
