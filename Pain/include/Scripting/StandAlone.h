#pragma once
#include <SDL2/SDL_events.h>
#include <lua.h>

namespace pain
{
struct LuaScriptComponent {
  const char *m_scriptPath;
  lua_State *m_L = nullptr;
  LuaScriptComponent() = default;
  LuaScriptComponent(const char *scriptPath);
  ~LuaScriptComponent();
  void onCreate();
  void onUpdate(double deltaTime);
  void onDestroy();
  void onEvent(const SDL_Event *event);
  int standAlone();

private:
  void callIfExists(const char *name);
  void stackDump(lua_State *L);
};
} // namespace pain
