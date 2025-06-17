#include "Scripting/Component.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreFiles/ResourceManagerSing.h"
#include "ECS/Scriptable.h"
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <sol/types.hpp>

namespace pain
{
void custom_print(const std::string &str) { PLOG_I("{}", str); }

LuaScriptComponent::LuaScriptComponent(const char *scriptPath,
                                       sol::state &solState)
    : m_scriptPath(scriptPath), m_lua(solState),
      m_env(solState, sol::create, solState.globals())
{
  sol::load_result &script = resources::loadLuaFile(m_lua, m_scriptPath);
  if (!script.valid()) {
    sol::error err = script;
    PLOG_E("Error loading Lua script: {}", err.what());
  }

  sol::protected_function_result result =
      script(); // will place everything inside an anonymous function and run
  if (!result.valid()) {
    sol::error err = result;
    PLOG_E("Error loading lua script: {}", err.what());
    return;
  }
}
void LuaScriptComponent::InitializeScript()
{
  // NOTE: instead of setting an environment, there is also the option of
  // setting an new custom LuaScriptComponent usertype which stores the local
  // stuff. Not that will have any difference imho. It's just another philosophy
  // that accomplishes the same goal
  set_environment(m_env, resources::loadLuaFile(m_lua, m_scriptPath)());
  // Store references to possible callbacks
  m_onUpdate = m_env["onUpdate"];
  m_onCreate = m_env["onCreate"];
  m_onDestroy = m_env["onDestroy"];

  if (hasAnyComponents<TransformComponent>()) {
    setTemplateFunction<TransformComponent>("getPosition");
  }
  if (hasAnyComponents<MovementComponent>()) {
    setTemplateFunction<MovementComponent>("getMovement");
  }
  if (hasAnyComponents<SpriteComponent>()) {
    setTemplateFunction<SpriteComponent>("getSprite");
  }
}

void LuaScriptComponent::onCreate()
{
  if (m_onCreate) {
    sol::protected_function_result result = m_onCreate();
    if (!result.valid()) {
      PLOG_E("Lua error (onCreate): {}", result.get<sol::error>().what());
    }
  }
}
void LuaScriptComponent::onUpdate(double deltaTime)
{
  if (m_onUpdate) {
    sol::protected_function_result result = m_onUpdate(deltaTime);
    if (!result.valid()) {
      PLOG_E("Lua error (onUpdate): {}", result.get<sol::error>().what());
    }
  }
}
void LuaScriptComponent::onDestroy()
{
  if (m_onDestroy) {
    sol::protected_function_result result = m_onDestroy();
    if (!result.valid()) {
      PLOG_E("Lua error (onDestroy): {}", result.get<sol::error>().what());
    }
  }
}
void LuaScriptComponent::onEvent(const SDL_Event *event)
{
  // not implemented yet
}

LuaScriptComponent &
LuaScriptComponent::operator=(LuaScriptComponent &&other) noexcept
{
  if (this != &other) {
    m_lua = std::move(other.m_lua);
    m_scriptPath = std::move(other.m_scriptPath);
  }
  return *this;
}

} // namespace pain
