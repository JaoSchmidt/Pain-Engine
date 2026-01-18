/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "Scripting/LuaScriptSys.h"
#include "Debugging/Profiling.h"
#include "Scripting/Component.h"
#include <CoreRender/Renderer/Renderer2d.h>
#include <sol/error.hpp>
#include <sol/protected_function_result.hpp>

// NOTE: I believe lua script can be tough to paralize because of the OOP nature
// of scripts
namespace pain
{
namespace Systems
{

void LuaScript::onUpdate(DeltaTime deltaTime)
{
  PROFILE_SCOPE("onUpdate - LuaScripts");

  auto chunks = query<LuaScriptComponent>();

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &lsc = scripts[i];

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
}

void LuaScript::onEvent(const SDL_Event &e)
{
  PROFILE_SCOPE("onEvent - LuaScripts");

  auto chunks = query<LuaScriptComponent>();

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &lsc = scripts[i];

      if (lsc.m_onEventFunction) {
        sol::protected_function_result result =
            (*lsc.m_onEventFunction)(lsc, e);

        if (!result.valid()) {
          PLOG_E("Lua error (m_onEventFunction): {}",
                 result.get<sol::error>().what());
        }
      }
    }
  }
}

void LuaScript::onRender(Renderer2d &renderer, bool isMinimized,
                         DeltaTime currentTime)
{
  PROFILE_SCOPE("onRender - LuaScripts");

  auto chunks = query<LuaScriptComponent>();

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &lsc = scripts[i];

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
}

} // namespace Systems
} // namespace pain
