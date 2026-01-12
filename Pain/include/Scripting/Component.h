#pragma once
#include "Core.h"
#include <SDL2/SDL_events.h>

#include "Assets/ResourceManager.h"
#include "ECS/Components/ComponentManager.h"
#include <sol/sol.hpp>

namespace pain
{
struct LuaScriptComponent {
  using tag = tag::LuaScript;
  static LuaScriptComponent create(reg::Entity entity);
  void bind(sol::state &solstate, const char *scriptPath);

  std::optional<sol::protected_function> m_onCreate;
  std::optional<sol::protected_function> m_onUpdateFunction;
  std::optional<sol::protected_function> m_onEventFunction;
  std::optional<sol::protected_function> m_onRenderFunction;
  std::optional<sol::protected_function> m_onDestroy;
  const char *m_scriptPath = resources::getDefaultLuaFile();
  reg::Entity entity;

  LuaScriptComponent(reg::Entity entity);
};

} // namespace pain
