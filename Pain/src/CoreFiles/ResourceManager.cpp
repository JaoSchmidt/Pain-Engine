#include "Assets/ResourceManager.h"
#include "Assets/IniWrapper.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Texture.h"
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

namespace fs = std::filesystem;

namespace
{
// NOTE: remember folks, surfaceMap is in the static/global memory but its
// content is in the heap
static std::map<std::string, std::string> m_luaScriptSource = {};
} // namespace
namespace pain
{
bool resources::existsFile(const std::string_view &name)
{
  return std::filesystem::exists(std::filesystem::path{name});
}
bool resources::existsFile(const char *name)
{
  struct stat buffer;
  return (stat(name, &buffer) == 0);
}
bool resources::existsFile(const std::string &name)
{
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}
bool resources::isDir(const std::string &name)
{
  return fs::is_directory(name);
}

bool resources::isFile(const std::string &name)
{
  return fs::is_regular_file(name);
}

bool resources::createFile(const std::string &filename)
{
  std::ofstream ofs(filename.c_str());
  if (!ofs) {
    PLOG_E("Error: Could not create file {}", filename);
    return false;
  }
  return true;
}

bool resources::isSettingsGuiNeeded()
{
  if (!std::filesystem::exists("config.ini"))
    return true;

  mINI::INIFile file("config.ini");
  mINI::INIStructure ini;

  if (!file.read(ini))
    return true;

  return !IniWrapper::getBoolean(ini, "settings", "HideConfig", true);
}

// load a script inside filepath, if it's not already loaded, created
const std::string &resources::getLuaScriptSource(const char *filepath)
{
  auto it = m_luaScriptSource.find(filepath);
  if (it != m_luaScriptSource.end())
    return it->second;

  if (!existsFile(filepath)) {
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

void resources::initiateDefaultScript()
{
  getLuaScriptSource(getDefaultLuaFile());
}

const char *resources::getDefaultLuaFile()
{
  return "resources/default/scripts/default.lua";
}

std::string resources::getCurrentWorkingDir(std::string append)
{
  char sep =
#ifdef PLATFORM_IS_WINDOWS
      '\\';
#else
      '/';
#endif
  char buff[FILENAME_MAX];
  getcwd(buff, FILENAME_MAX);
  const std::string currentDir(buff);
  return currentDir + sep + append;
}

std::string resources::getCurrentWorkingDir()
{
  char buff[FILENAME_MAX];
  getcwd(buff, FILENAME_MAX);
  const std::string currentDir(buff);
  return currentDir;
}

void resources::clearScript() { m_luaScriptSource.clear(); }

} // namespace pain
