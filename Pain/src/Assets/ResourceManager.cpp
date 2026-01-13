#include "Assets/ResourceManager.h"
#include "CoreFiles/LogWrapper.h"

#include <filesystem>
namespace pain
{

void IConfig::readAndUpdate(const char *dirname, const char *fileName)
{
  fs::path fullPath = dirname;
  fullPath /= fileName;
  readAndUpdate(fullPath.c_str());
}

void InternalConfig::readAndUpdate(const char *filename)
{
  if (!resources::existsFile(filename) || !resources::isFile(filename)) {
    if (!resources::isDir(filename)) {
      PLOG_W("internalSettings.ini not found, generating a new one");
      resources::createFile(filename);
    } else
      std::terminate();

    write(filename);
    return;
  }

  mINI::INIFile file(filename);
  mINI::INIStructure ini;
  file.read(ini);

  zoomLevel.initValue(ini, INTERNAL_SETTINGS_STRING);
  gridSize.initValue(ini, INTERNAL_SETTINGS_STRING);
  title.initValue(ini, INTERNAL_SETTINGS_STRING);
  swapChainTarget.initValue(ini, INTERNAL_SETTINGS_STRING);
}

void InternalConfig::write(const char *filename)
{
  mINI::INIFile file(filename);
  mINI::INIStructure ini;

  file.read(ini);
  zoomLevel.writeBuffer(ini, INTERNAL_SETTINGS_STRING);
  gridSize.writeBuffer(ini, INTERNAL_SETTINGS_STRING);
  title.writeBuffer(ini, INTERNAL_SETTINGS_STRING);
  swapChainTarget.writeBuffer(ini, INTERNAL_SETTINGS_STRING);
  file.write(ini);
}

void IniConfig::readAndUpdate(const char *filename)
{
  if (!resources::existsFile(filename) || !resources::isFile(filename)) {
    resources::createFile(filename);
    write(filename);
    return;
  }

  mINI::INIFile file(filename);
  mINI::INIStructure ini;
  file.read(ini);

  hideConfig.initValue(ini, SETTINGS_STRING);
  fullscreen.initValue(ini, SETTINGS_STRING);
  assetsPath.initValue(ini, SETTINGS_STRING);
  defaultHeight.initValue(ini, SETTINGS_STRING);
  defaultWidth.initValue(ini, SETTINGS_STRING);
}

void IniConfig::write(const char *filename)
{
  mINI::INIFile file(filename);
  mINI::INIStructure ini;

  file.read(ini);
  hideConfig.writeBuffer(ini, SETTINGS_STRING);
  fullscreen.writeBuffer(ini, SETTINGS_STRING);
  assetsPath.writeBuffer(ini, SETTINGS_STRING);
  defaultHeight.writeBuffer(ini, SETTINGS_STRING);
  defaultWidth.writeBuffer(ini, SETTINGS_STRING);
  file.write(ini);
}

} // namespace pain
