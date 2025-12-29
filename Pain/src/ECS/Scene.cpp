// Scene.cpp
#include "ECS/Scene.h"

#include "CoreRender/RenderSys.h"
#include "Debugging/Profiling.h"
#include "GUI/ImGuiSys.h"
#include "Misc/Events.h"
#include "Scripting/Component.h"
#include "Scripting/LuaScriptSys.h"
#include "Scripting/NativeScriptSys.h"
#include <concepts>

#include "CoreRender/RenderSys.h"

namespace pain
{
// ------------------------------------------------
// event - lua bridge
// ------------------------------------------------
template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::emplaceLuaScript(reg::Entity entity,
                                              const char *scriptPath)
  requires(Manager::template isRegistered<LuaScriptComponent>())
{
  LuaScriptComponent &lc = getComponent<LuaScriptComponent>(entity);
  lc.bind(m_luaState, scriptPath);
  if (lc.m_onCreate) {
    sol::protected_function_result result = (*lc.m_onCreate)(lc);
    if (!result.valid()) {
      PLOG_E("Lua error (m_onUpdateFunction): {}",
             result.get<sol::error>().what());
    }
  }
}

// ------------------------------------------------
template <reg::CompileTimeBitMaskType Manager>
AbstractScene<Manager>::AbstractScene(reg::EventDispatcher &ed,
                                      sol::state &solState)

    : m_registry(), m_eventDispatcher(ed),
      m_luaState(createScriptEventMap(solState, m_eventDispatcher)),
      m_entity(createEntity()){};

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::updateSystems(DeltaTime deltaTime)
{
  PROFILE_FUNCTION();
  for (auto *sys : m_updateSystems)
    static_cast<IOnUpdate *>(sys)->onUpdate(deltaTime);
  m_eventDispatcher.update();
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::updateSystems(const SDL_Event &event)
{
  PROFILE_SCOPE("Scene::updateSystems - events for nsc");
  for (auto *sys : m_eventSystems)
    static_cast<IOnEvent *>(sys)->onEvent(event);
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::renderSystems(Renderer2d &renderer,
                                           bool isMinimized,
                                           DeltaTime currentTime)
{
  for (auto *sys : m_renderSystems)
    static_cast<IOnRender *>(sys)->onRender(renderer, isMinimized, currentTime);
}

template class AbstractScene<ComponentManager>;
template class AbstractScene<UIManager>;

using Scene = AbstractScene<ComponentManager>;
using UIScene = AbstractScene<UIManager>;
} // namespace pain
