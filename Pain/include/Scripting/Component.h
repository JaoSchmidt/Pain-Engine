#pragma once
#include "Core.h"
#include "ECS/Scriptable.h"
#include <SDL2/SDL_events.h>

#include "Assets/ResourceManager.h"
#include <sol/sol.hpp>
namespace pain
{
struct LuaScriptComponent : public ExtendedEntity {
  LuaScriptComponent(Entity entity, Bitmask Bitmask, Scene &scene,
                     sol::state &solState);
  void bind(const char *scriptPath);
  void onCreate();
  void onDestroy();
  void onEvent(const SDL_Event *event);
  sol::state &getLuaState() { return m_lua; };

  NONCOPYABLE(LuaScriptComponent);
  LuaScriptComponent &operator=(LuaScriptComponent &&other) noexcept;
  LuaScriptComponent(LuaScriptComponent &&other) noexcept;
  ~LuaScriptComponent() = default;
  void initializeScript();

  std::optional<sol::protected_function> m_onCreate;
  std::optional<sol::protected_function> m_onUpdateFunction;
  std::optional<sol::protected_function> m_onEventFunction;
  std::optional<sol::protected_function> m_onRenderFunction;
  std::optional<sol::protected_function> m_onDestroy;
  std::optional<sol::protected_function> m_onCollisionEnter;
  std::unordered_set<Entity> m_currentCollisions;

private:
  const char *m_scriptPath = resources::getDefaultLuaFile();
  sol::state &m_lua;
};

} // namespace pain
