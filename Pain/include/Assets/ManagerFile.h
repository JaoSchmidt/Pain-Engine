#pragma once
#include "Core.h"
#include <filesystem>

namespace pain
{

// The resources manager. A simple API to concentrate all interactions with
// heavy resoruces like files while also avoiding potentials fatal errors
namespace FileManager
{

// =================================================================
// Assure default reducancy
// =================================================================
void initiateDefaultScript();
const std::string &getDefaultPath();
void setDefaultPath(const char *path);
std::string getFullPath(const char *relativeOrAbsolute);
// equivalent to getcwd
std::string getCurrentWorkingDir();
// equivalent to getcwd + will append your path for you
std::string getCurrentWorkingDir(std::string append);

// =================================================================
// Lua Related Stuff
// =================================================================

const std::string &getLuaScriptSource(const char *filepath);
const char *getDefaultLuaFile();
void clearScript();

// =================================================================
// FileSystem wrappers
// =================================================================

bool existsFile(const char *name);
bool existsFile(const std::string &name);
bool existsFile(const std::string_view &name);
bool isFile(const std::string &name);
bool isDir(const std::string &name);
bool createFile(const std::string &filename);
template <typename... P> std::string pathJoin(const P &...parts)
{
  std::filesystem::path result;
  (result /= ... /= parts);
  return result.string();
}

} // namespace FileManager

} // namespace pain
