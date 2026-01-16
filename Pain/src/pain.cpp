/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "pain.h"
#include "Assets/ManagerIni.h"
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
  return IniConfig::isSettingsGuiNeeded();
}

EndGameFlags Pain::runAndDeleteApplication(Application *app)
{
  EndGameFlags flags = app->run();
  delete app;
  return flags;
}
} // namespace pain
