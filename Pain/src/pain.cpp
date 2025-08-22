#include "pain.h"
#include "Core.h"
#include <glad/gl.h>

namespace pain
{

bool Pain::initiate()
{
  // start log
  logWrapper::Init();
  PLOG_T("Initialized Log!");
  return resources::isSettingsGuiNeeded();
}

void Pain::runAndDeleteApplication(Application *app)
{
  Application *application = app;
  application->run();
  delete application;
}
} // namespace pain
