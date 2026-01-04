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

LuaScriptComponent LuaScriptComponent::create(reg::Entity entity, Scene &scene)
{
  return LuaScriptComponent(entity, scene);
};
LuaScriptComponent::LuaScriptComponent(reg::Entity entity, Scene &scene)
    : ExtendedEntity(entity, scene) {};

void LuaScriptComponent::bind(sol::state &lua, const char *scriptPath)
{

  sol::table script_api = lua.create_table();

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
  m_scriptPath = scriptPath;
  lua["Script"] = script_api;
  sol::load_result script =
      lua.load(resources::getLuaScriptSource(m_scriptPath), m_scriptPath);
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
} // namespace pain
