#pragma once
#include "ECS/Scriptable.h"
#include "pch.h"
#include <sol/state.hpp>

namespace pain
{

// The resources manager. A simple API to concentrate all interactions with
// heavy resoruces. While also avoiding potentials fatal errors
namespace resources
{
struct ConfigIni {
  bool showDebugMenu = false;
  bool hideConfigIniGui = false;
  std::string resourcesFolder = "resources";
  glm::ivec2 resolution = {0.f, 0.f};
};
ConfigIni &configDotIni();
bool isSettingsGuiNeeded();
SDL_Surface *getSurface(const char *filepath);

const char *getDefaultLuaFile();
const std::string &getLuaScriptSource(const char *filepath);
void initiateDefaultResources();
void initiateDefaultScript(sol::state &solstate);
const std::string getCurrentWorkingDir();

class defaultNativeScript : public ExtendedEntity
{
  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &e);
  void onRender();
  void onDestroy();
};

} // namespace resources
} // namespace pain
