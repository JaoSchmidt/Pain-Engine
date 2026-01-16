// ResourceManager.cpp
#include "Assets/ManagerFile.h"
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
std::string m_resourcesPath; // we can replace this with a depencency injection
                             // of IniConfig but not really in a hurry tbh
static std::map<std::string, std::string> m_luaScriptSource = {};
} // namespace

namespace pain
{

// -----------------------------------------------------------------
// start the stuff to run settings app
// -----------------------------------------------------------------

void FileManager::initiateDefaultScript()
{
  getLuaScriptSource(getDefaultLuaFile());
}

// =================================================================
// Assure default reducancy
// =================================================================

const std::string &FileManager::getDefaultPath() { return m_resourcesPath; }

void FileManager::setDefaultPath(const char *path)
{
  namespace fs = std::filesystem;
  fs::path p(path);
  // normalize path (remove redundant ./ and ensure trailing slash)
  p = fs::weakly_canonical(p);
  if (p.filename() == "resources") {
    p = p.parent_path();
  }
  m_resourcesPath = p.string();
}

std::string FileManager::getFullPath(const char *relativeOrAbsolute)
{
  namespace fs = std::filesystem;
  fs::path p(relativeOrAbsolute);

  // If already absolute, just normalize it
  if (p.is_absolute()) {
    return fs::weakly_canonical(p).string();
  }

  fs::path base(m_resourcesPath);
  fs::path full = base / p;
  return fs::weakly_canonical(full).string();
}

std::string FileManager::getCurrentWorkingDir(std::string append)
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

std::string FileManager::getCurrentWorkingDir()
{
  char buff[FILENAME_MAX];
  getcwd(buff, FILENAME_MAX);
  const std::string currentDir(buff);
  return currentDir;
}

// =================================================================
// Lua Related Stuff
// =================================================================

// load a script inside filepath, if it's not already loaded, created
const std::string &FileManager::getLuaScriptSource(const char *filepath)
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

const char *FileManager::getDefaultLuaFile()
{
  return "resources/default/scripts/default.lua";
}

void FileManager::clearScript() { m_luaScriptSource.clear(); }

// =================================================================
// FileSystem wrappers
// =================================================================

bool FileManager::existsFile(const std::string_view &name)
{
  return std::filesystem::exists(std::filesystem::path{name});
}
bool FileManager::existsFile(const char *name)
{
  struct stat buffer;
  return (stat(name, &buffer) == 0);
}
bool FileManager::existsFile(const std::string &name)
{
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}
bool FileManager::isDir(const std::string &name)
{
  return fs::is_directory(name);
}

bool FileManager::isFile(const std::string &name)
{
  return fs::is_regular_file(name);
}

bool FileManager::createFile(const std::string &filename)
{
  std::ofstream ofs(filename.c_str());
  if (!ofs) {
    PLOG_E("Error: Could not create file {}", filename);
    return false;
  }
  return true;
}

} // namespace pain
