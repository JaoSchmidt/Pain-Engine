#include "pain.h"
#include "Core.h"
#include "CoreFiles/EndGameFlags.h"
#include <glad/gl.h>

namespace pain
{

bool Pain::initiate()
{
  // start log
  logWrapper::InitLogger();
  PLOG_T("Initialized Log!");
  return resources::isSettingsGuiNeeded();
}

EndGameFlags Pain::runAndDeleteApplication(Application *app)
{
  EndGameFlags flags = app->run();
  delete app;
  return flags;
}
} // namespace pain
