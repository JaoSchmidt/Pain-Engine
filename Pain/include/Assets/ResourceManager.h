#pragma once
#include "ECS/Scriptable.h"
#include "pch.h"
#include <sol/state.hpp>

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
void initiateDefaultScript(sol::state &solstate);
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
  const T def;
  T value = def;
  const char *name;
  Config(T t, const char *name) : def(t), name(name) {}
  Config &operator=(Config &&o)
  {
    name = o.name;
    value = o.value;
    return *this;
  }
  Config(Config &&o) : def(o.def), name(o.name), value(o.value) {}
  NONCOPYABLE(Config);
};
struct IniConfig {
  Config<bool> hideConfig{false, "HideConfig"};
  Config<bool> fullscreen{false, "Fullscreen"};
  Config<std::string> assetsPath{
      pain::resources::getCurrentWorkingDir("resources"), "AssetPath"};

  IniConfig(IniConfig &&) = default;
  IniConfig &operator=(IniConfig &&) = default;
  IniConfig() = default;
  ~IniConfig() = default;
  NONCOPYABLE(IniConfig)
};

} // namespace pain
