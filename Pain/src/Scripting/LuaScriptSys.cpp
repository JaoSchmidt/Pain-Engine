#include "Scripting/LuaScriptSys.h"
#include "Debugging/Profiling.h"
#include "Scripting/Component.h"
#include <CoreRender/Renderer/Renderer2d.h>
#include <sol/error.hpp>
#include <sol/protected_function_result.hpp>

namespace pain
{
namespace Systems
{

void LuaScript::onUpdate(double deltaTime)
{
  PROFILE_SCOPE("onUpdate - LuaScripts");
  for (auto it = begin<LuaScriptComponent>(); it != end<LuaScriptComponent>();
       ++it) {
    auto &lsc = *it;
    if (lsc.m_onUpdateFunction) {
      sol::protected_function_result result =
          (*lsc.m_onUpdateFunction)(lsc, deltaTime);
      if (!result.valid()) {
        PLOG_E("Lua error (m_onUpdateFunction): {}",
               result.get<sol::error>().what());
      }
    }
  }
}
void LuaScript::onEvent(const SDL_Event &e)
{
  PROFILE_SCOPE("onEvent - LuaScripts");
  for (auto it = begin<LuaScriptComponent>(); it != end<LuaScriptComponent>();
       ++it) {
    auto &lsc = *it;
    if (lsc.m_onEventFunction) {
      sol::protected_function_result result = (*lsc.m_onEventFunction)(lsc, e);
      if (!result.valid()) {
        PLOG_E("Lua error (m_onEventFunction): {}",
               result.get<sol::error>().what());
      }
    }
  }
}
void LuaScript::onRender(Renderer2d &renderer, bool isMinimized,
                         double currentTime)
{
  PROFILE_SCOPE("onRender - LuaScripts");
  for (auto it = begin<LuaScriptComponent>(); it != end<LuaScriptComponent>();
       ++it) {
    auto &lsc = *it;

    if (lsc.m_onRenderFunction) {
      sol::protected_function_result result =
          (*lsc.m_onRenderFunction)(lsc, renderer, isMinimized, currentTime);
      if (!result.valid()) {
        PLOG_E("Lua error (m_onRenderFunction): {}",
               result.get<sol::error>().what());
      }
    }
  }
}
} // namespace Systems
} // namespace pain
