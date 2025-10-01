#pragma once
#include "Assets/IniWrapper.h"
#include "ECS/Scriptable.h"
#include "mini/ini.h"
#include "pch.h"
#include <sol/state.hpp>
#include <utility>

namespace pain
{

// The resources manager. A simple API to concentrate all interactions with
// heavy resoruces like files while also avoiding potentials fatal errors
namespace resources
{

bool isSettingsGuiNeeded();
SDL_Surface *getSurface(const char *filepath);

const char *getDefaultLuaFile();
const std::string &getLuaScriptSource(const char *filepath);
void initiateDefaultScript(const sol::state &solstate);
std::string getCurrentWorkingDir();
std::string getCurrentWorkingDir(std::string append);
bool exists_file(const std::string &name);
void clearScript();

class defaultNativeScript : public ExtendedEntity
{
  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &e);
  void onRender();
  void onDestroy();
};

} // namespace resources
template <typename T> struct Config {
private:
  T m_def;

public:
  const T getDefault() const { return std::as_const(m_def); }
  T value = m_def;
  const char *name;
  Config(T t, const char *name) : m_def(t), value(m_def), name(name) {}
  Config &operator=(Config &&o)
  {
    if (this != &o) {
      m_def = o.m_def;
      name = o.name;
      value = o.value;
    }
    return *this;
  }
  Config(Config &&o) : m_def(o.m_def), value(o.value), name(o.name) {}
  NONCOPYABLE(Config);
};
struct IniConfig {
  Config<bool> hideConfig{false, "HideConfig"};
  Config<bool> fullscreen{false, "Fullscreen"};
  Config<std::string> assetsPath{
      pain::resources::getCurrentWorkingDir("resources"), "AssetPath"};
  void readAndUpdate(const char *filename)
  {
    mINI::INIFile file(filename);
    mINI::INIStructure ini;
    file.read(ini);
    // clang-format off
    hideConfig.value = IniWrapper::getBoolean(
        ini, "settings", hideConfig.name, hideConfig.getDefault());
    fullscreen.value = IniWrapper::getBoolean(
        ini, "settings", fullscreen.name, fullscreen.getDefault());
    assetsPath.value = IniWrapper::get(
        ini, "settings", assetsPath.name, assetsPath.getDefault());
    // clang-format on
  }
  void write(const char *filename)
  {
    mINI::INIFile file(filename);
    mINI::INIStructure ini;
    file.read(ini);
    ini["settings"][hideConfig.name] = hideConfig.value ? "true" : "false";
    ini["settings"][fullscreen.name] = fullscreen.value ? "true" : "false";
    ini["settings"][assetsPath.name] = assetsPath.value;
    file.write(ini);
  }

  IniConfig(IniConfig &&) = default;
  IniConfig &operator=(IniConfig &&) = default;
  IniConfig() = default;
  ~IniConfig() = default;
  NONCOPYABLE(IniConfig)
};

} // namespace pain
