#include "Scripting/Component.h"
#include "Assets/ResourceManager.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Scriptable.h"
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <sol/types.hpp>

namespace pain
{
void custom_print(const std::string &str) { PLOG_I("{}", str); }

LuaScriptComponent::LuaScriptComponent(Entity entity, Bitmask bitmask,
                                       Scene &scene, sol::state &solState)
    : ExtendedEntity(entity, bitmask, scene), m_lua(solState) {};
void LuaScriptComponent::initializeScript() {}
void LuaScriptComponent::bind(const char *scriptPath)
{

  sol::table script_api = m_lua.create_table();

  // Temporary table that store references to possible callbacks... lambda is
  // only invoked if the function exists inside lua script
  script_api["on_create"] = [&](sol::function f) {
    m_onCreate = sol::protected_function(std::move(f));
  };
  script_api["on_update"] = [&](sol::function f) {
    m_onUpdateFunction = sol::protected_function(std::move(f));
  };
  script_api["on_event"] = [&](sol::function f) {
    m_onEventFunction = sol::protected_function(std::move(f));
  };
  script_api["on_render"] = [&](sol::function f) {
    m_onRenderFunction = sol::protected_function(std::move(f));
  };
  script_api["on_destroy"] = [&](sol::function f) {
    m_onDestroy = sol::protected_function(std::move(f));
  };
  script_api["on_collision_enter"] = [&](sol::function f) {
    m_onCollisionEnter = sol::protected_function(std::move(f));
  };

  m_scriptPath = scriptPath;
  m_lua["Script"] = script_api;
  sol::load_result script =
      m_lua.load(resources::getLuaScriptSource(m_scriptPath), m_scriptPath);
  if (!script.valid()) {
    sol::error err = script;
    PLOG_E("Error loading Lua script: {}", err.what());
  }
  PLOG_I("has transform? {}", hasAnyComponents<TransformComponent>());
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
    sol::protected_function_result result = (*m_onCreate)(*this);
    if (!result.valid()) {
      PLOG_E("Lua error (onCreate): {}", result.get<sol::error>().what());
    }
  }
}
void LuaScriptComponent::onDestroy()
{
  if (m_onDestroy) {
    sol::protected_function_result result = (*m_onDestroy)(*this);
    if (!result.valid()) {
      PLOG_E("Lua error (onDestroy): {}", result.get<sol::error>().what());
    }
  }
}

LuaScriptComponent::LuaScriptComponent(LuaScriptComponent &&other) noexcept
    : ExtendedEntity(std::move(other)), // base class
      m_onCreate(std::move(other.m_onCreate)),
      m_onDestroy(std::move(other.m_onDestroy)),
      m_scriptPath(std::exchange(other.m_scriptPath, nullptr)),
      m_lua(other.m_lua) {};

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
