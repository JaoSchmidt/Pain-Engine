#pragma once
#include "Core.h"
#include "ECS/Scriptable.h"
#include <SDL2/SDL_events.h>

#include "Assets/ResourceManager.h"
#include <sol/sol.hpp>
namespace pain
{
struct LuaScriptComponent : public ExtendedEntity {
  LuaScriptComponent(reg::Entity entity, Scene &scene);
  void bind(sol::state &solstate, const char *scriptPath);

  NONCOPYABLE(LuaScriptComponent);
  LuaScriptComponent &operator=(LuaScriptComponent &&other) = delete;
  LuaScriptComponent(LuaScriptComponent &&other) noexcept;
  ~LuaScriptComponent() = default;

  std::optional<sol::protected_function> m_onCreate;
  std::optional<sol::protected_function> m_onUpdateFunction;
  std::optional<sol::protected_function> m_onEventFunction;
  std::optional<sol::protected_function> m_onRenderFunction;
  std::optional<sol::protected_function> m_onDestroy;

private:
  const char *m_scriptPath = resources::getDefaultLuaFile();
};

} // namespace pain
