#include "Assets/ResourceManager.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Texture.h"
#include <INIReader.h>
#include <cstdio>
#include <iostream>
#include <stdio.h> /* defines FILENAME_MAX */
#include <utility>

#ifdef PLATFORM_IS_WINDOWS
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

namespace pain
{
inline bool exists_file(const std::string &name)
{
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}
bool resources::isSettingsGuiNeeded()
{
  if (!exists_file("config.ini"))
    return true;
  INIReader reader("config.ini");
  return reader.GetBoolean("settings", "hideConfigIniGui", true);
}

static resources::ConfigIni configIni;
resources::ConfigIni &resources::configDotIni() { return configIni; }
// NOTE: remember folks, surfaceMap is in the static/global memory but its
// content are in the heap
static std::map<const char *, sol::load_result> m_luaFileMap = {};
static std::map<const char *, std::string> m_luaScriptSource;

// load a script inside filepath, if it's not already loaded, created
const std::string &resources::getLuaScriptSource(const char *filepath)
{
  auto it = m_luaScriptSource.find(filepath);
  if (it != m_luaScriptSource.end())
    return it->second;

  if (!exists_file(filepath)) {
    PLOG_E("Could not locate filepath {}", filepath);
    auto it = m_luaScriptSource.find(getDefaultLuaFile());
    P_ASSERT(it != m_luaScriptSource.end(),
             "Could not load default script file {}", getDefaultLuaFile());
    return it->second;
  }

  std::ifstream in(filepath);
  std::string line;
  std::stringstream ss;

  while (getline(in, line)) {
    ss << line << '\n';
  }

  m_luaScriptSource.emplace(filepath, ss.str());
  return m_luaScriptSource[filepath];
}

// start the stuff to run settings app
void resources::initiateDefaultResources() {}

void resources::initiateDefaultScript(sol::state &solstate)
{
  getLuaScriptSource(getDefaultLuaFile());
}

const char *resources::getDefaultLuaFile()
{
  return "resources/default/scripts/default.lua";
}

const std::string resources::getCurrentWorkingDir()
{
  char buff[FILENAME_MAX];
  getcwd(buff, FILENAME_MAX);
  const std::string currentDir(buff);
  return currentDir;
}

void resources::defaultNativeScript::onUpdate(double deltaTimeSec)
{
  PLOG_W("You are updating a script that hasn't been initialized!");
}

void resources::defaultNativeScript::onEvent(const SDL_Event &e)
{
  PLOG_W("You are updating a script that hasn't been initialized!");
}
void resources::defaultNativeScript::onRender()
{
  PLOG_W("You are trying to render a script that hasn't been initialized!");
}
void resources::defaultNativeScript::onDestroy()
{
  PLOG_W("You are trying to destroy the default script, not sure if that is "
         "suppose to happen");
}

} // namespace pain
