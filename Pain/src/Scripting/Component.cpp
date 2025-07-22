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

LuaScriptComponent::LuaScriptComponent(sol::state &solState) : m_lua(solState)
{
}
void LuaScriptComponent::initializeScript() {}
void LuaScriptComponent::bind(const char *scriptPath)
{

  sol::table script_api = m_lua.create_table();

  // Temporary table that store references to possible callbacks... lambda is
  // only invoked if the function exists inside lua script
  script_api["on_create"] = [&](sol::function f) {
    *m_onCreate = std::move(f);
  };
  m_onUpdateFunction = m_scene->createEntity();
  script_api["on_update"] = [&](sol::function f) {
    m_scene->createComponent<onUpdateLuaFunction>(m_onUpdateFunction,
                                                  std::move(f));
  };
  script_api["on_destroy"] = [&](sol::function f) {
    *m_onDestroy = std::move(f);
  };

  if (hasAnyComponents<TransformComponent>())
    m_lua.set_function("getPosition",
                       [](LuaScriptComponent &c) -> TransformComponent & {
                         return c.getComponent<TransformComponent>();
                       });
  if (hasAnyComponents<MovementComponent>())
    m_lua.set_function("getMovement",
                       [](LuaScriptComponent &c) -> MovementComponent & {
                         return c.getComponent<MovementComponent>();
                       });
  if (hasAnyComponents<SpriteComponent>())
    m_lua.set_function("getSprite",
                       [](LuaScriptComponent &c) -> SpriteComponent & {
                         return c.getComponent<SpriteComponent>();
                       });
  m_scriptPath = scriptPath;
  m_lua["script"] = script_api;
  sol::load_result script =
      m_lua.load(resources::getLuaScriptSource(m_scriptPath));
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

void LuaScriptComponent::onCreate()
{
  if (m_onCreate) {
    sol::protected_function_result result = (*m_onCreate)();
    if (!result.valid()) {
      PLOG_E("Lua error (onCreate): {}", result.get<sol::error>().what());
    }
  }
}
void LuaScriptComponent::onDestroy()
{
  if (m_onDestroy) {
    sol::protected_function_result result = (*m_onDestroy)();
    if (!result.valid()) {
      PLOG_E("Lua error (onUpdate): {}", result.get<sol::error>().what());
    }
  }
}

void LuaScriptComponent::onEvent(const SDL_Event *event)
{
  // not implemented yet
}

LuaScriptComponent::LuaScriptComponent(LuaScriptComponent &&other) noexcept
    : ExtendedEntity(std::move(other)), // base class
      m_scriptPath(std::exchange(other.m_scriptPath, nullptr)),
      m_lua(other.m_lua), m_onCreate(std::move(other.m_onCreate)),
      m_onDestroy(std::move(other.m_onDestroy))
{
}

LuaScriptComponent &
LuaScriptComponent::operator=(LuaScriptComponent &&other) noexcept
{
  if (this != &other) {
    ExtendedEntity::operator=(std::move(other)); // assign base

    m_scriptPath = std::exchange(other.m_scriptPath, nullptr);
    m_onCreate = std::move(other.m_onCreate);
    m_onDestroy = std::move(other.m_onDestroy);
    // m_lua is a reference, no need to assign again
  }
  return *this;
}
} // namespace pain
