#pragma once
#include "Core.h"
#include "ECS/Scriptable.h"
#include <SDL2/SDL_events.h>

#include <sol/sol.hpp>
namespace pain
{
struct LuaScriptComponent : public ExtendedEntity {
  LuaScriptComponent(sol::state &solState);
  void bind(const char *scriptPath);
  void onCreate();
  void onDestroy();
  void onEvent(const SDL_Event *event);

  NONCOPYABLE(LuaScriptComponent)
  LuaScriptComponent &operator=(LuaScriptComponent &&other) noexcept;
  LuaScriptComponent(LuaScriptComponent &&other) noexcept;
  ~LuaScriptComponent() = default;
  void initializeScript();

private:
  const char *m_scriptPath = resources::getDefaultLuaFile();
  sol::state &m_lua;

  Entity m_onUpdateFunction = 0;
  sol::function *m_onCreate = nullptr;
  sol::function *m_onDestroy = nullptr;
};

} // namespace pain
