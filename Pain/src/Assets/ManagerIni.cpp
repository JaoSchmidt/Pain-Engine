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


#include "Assets/ManagerIni.h"
#include "Assets/ManagerFile.h"
#include "CoreFiles/LogWrapper.h"

#include <filesystem>

#define SETTINGS_STRING "settings"
#define SETTINGS_FILE "config.ini"
#define INTERNAL_SETTINGS_STRING "internalSettings"
#define INTERNAL_SETTINGS_FILE "internalConfig.ini"

namespace fs = std::filesystem;
namespace pain
{
// -----------------------------------------------------------------
// Internal Config
// -----------------------------------------------------------------

bool IniConfig::isSettingsGuiNeeded()
{
  if (!FileManager::existsFile("config.ini"))
    return true;

  mINI::INIFile file("config.ini");
  mINI::INIStructure ini;

  if (!file.read(ini))
    return true;

  return !IniWrapper::getBoolean(ini, "settings", "HideConfig", true);
}

void InternalConfig::readAndUpdate(const std::string &resourceFolder)
{
  fs::path pathVar = resourceFolder;
  pathVar /= INTERNAL_SETTINGS_FILE;
  const std::string fullPath = pathVar.string();

  if (!FileManager::existsFile(fullPath) || !FileManager::isFile(fullPath)) {
    if (FileManager::isDir(resourceFolder)) {
      PLOG_W("{} not found, generating a new one", INTERNAL_SETTINGS_FILE);
      FileManager::createFile(fullPath);
    } else
      std::terminate();

    write(fullPath);
    return;
  }

  mINI::INIFile file(fullPath);
  mINI::INIStructure ini;
  file.read(ini);

#define X(type, variable, default, name)                                       \
  variable.initValue(ini, INTERNAL_SETTINGS_STRING)
  INTERNAL_INI_CONFIGS
#undef X
}

void InternalConfig::write(const std::string &filename)
{
  mINI::INIFile file(filename);
  mINI::INIStructure ini;

  file.read(ini);
#define X(type, variable, default, name)                                       \
  variable.writeBuffer(ini, INTERNAL_SETTINGS_STRING)
  INTERNAL_INI_CONFIGS
#undef X
  file.write(ini);
}
#undef INTERNAL_INI_CONFIGS

// -----------------------------------------------------------------
// Ini Config
// -----------------------------------------------------------------

void IniConfig::readAndUpdate(bool isLauncher)
{
  if (!isLauncher && !FileManager::existsFile(SETTINGS_FILE)) {
    PLOG_W("config.ini file should exist at the beginning! Did you skip the "
           "launcher?");
    FileManager::createFile(SETTINGS_FILE);
    write(SETTINGS_FILE); // this create config.ini with default values
    return;
  }

  mINI::INIFile file(SETTINGS_FILE);
  mINI::INIStructure ini;
  file.read(ini);

#define X(type, variable, default, name)                                       \
  variable.initValue(ini, SETTINGS_STRING)
  INI_CONFIGS
#undef X

  FileManager::setDefaultPath(assetsPath.value.c_str());
}

void IniConfig::write(const std::string &filename)
{
  mINI::INIFile file(filename);
  mINI::INIStructure ini;

  file.read(ini);
#define X(type, variable, default, name)                                       \
  variable.writeBuffer(ini, SETTINGS_STRING)
  INI_CONFIGS
#undef X
  file.write(ini);
}

} // namespace pain
