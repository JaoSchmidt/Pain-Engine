/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Scene.cpp
#include "ECS/Scene.h"

#include "Debugging/Profiling.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Collision/SweepAndPruneSys.h"
#include "Scripting/Lua/LuaScriptComponent.h"
#include "Scripting/Lua/LuaScriptSys.h"

namespace
{

} // namespace
namespace pain
{
// ------------------------------------------------
// event - lua bridge
// ------------------------------------------------

// Allow components to have logic when they are added (to an entity)
template <typename Component, reg::CompileTimeBitMaskType Manager>
void onComponentAdded(AbstractScene<Manager> &scene, reg::Entity entity)
{
  if constexpr (std::is_same_v<Component, SAPCollider>) {
    Systems::SweepAndPruneSys *s =
        scene.template getSys<Systems::SweepAndPruneSys>();
    if (s) {
      s->insertCollider(entity);
    } else {
      PLOG_W(
          "You are trying to create Sweep and Prune component without adding "
          "a proper Sweep and Prune system");
    }
  }
}

// add component to an already existing archetype
template <typename T, reg::CompileTimeBitMaskType Manager>
void pushComponentInto(reg::ArcheRegistry<Manager> &registry,
                       reg::Entity entity, reg::Bitmask bitmask, T &&t)
{
  registry.manualPush(entity, bitmask, std::move(t));
}

template <reg::CompileTimeBitMaskType Manager>
sol::state &AbstractScene<Manager>::enchanceLuaState(sol::state &state)
{
  return state;
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::emplaceLuaScript( //
    reg::Entity entity,                        //
    AbstractScene<Manager> &scene,             //
    const char *scriptPath,                    //
    const sol::table &initArgs)
  requires(Manager::template isRegistered<tag::LuaScript>())
{
  LuaScriptComponent &lc = scene.getComponent<LuaScriptComponent>(entity);
  lc.bind(scene.m_luaState, scriptPath);
  if (lc.m_onCreate) {
    if (initArgs == sol::nil)
      PLOG_E("init args is nill on {}", scriptPath);
    sol::protected_function_result result =
        (*lc.m_onCreate)(lc.m_scriptTable, initArgs);
    if (!result.valid())
      PLOG_E("Lua error on create: {}", result.get<sol::error>().what());
  }
}
template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::emplaceLuaScript( //
    reg::Entity entity,                        //
    AbstractScene<Manager> &scene,             //
    const char *scriptPath)
  requires(Manager::template isRegistered<tag::LuaScript>())
{
  LuaScriptComponent &lc = scene.getComponent<LuaScriptComponent>(entity);
  lc.bind(scene.m_luaState, scriptPath);
  if (lc.m_onCreate) {
    sol::protected_function_result result = (*lc.m_onCreate)(lc.m_scriptTable);
    if (!result.valid())
      PLOG_E("Lua error on create: {}", result.get<sol::error>().what());
  }
}

// =============================================================== //
// =============================================================== //
// MULTI THREAD RELATED
// =============================================================== //
// =============================================================== //
template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::enqueueMainThread(MainThreadJob job)
{
  std::lock_guard lock(m_mainThreadMutex);
  m_mainThreadJobs.push(std::move(job));
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::flushMainThreadJobs()
{
  std::queue<MainThreadJob> jobs;

  {
    std::lock_guard lock(m_mainThreadMutex);
    std::swap(jobs, m_mainThreadJobs);
  }

  while (!jobs.empty()) {
    jobs.front()();
    jobs.pop();
  }
}

// =============================================================== //
// Constructors
// =============================================================== //

template <reg::CompileTimeBitMaskType Manager>
AbstractScene<Manager>::AbstractScene(reg::EventDispatcher &ed,
                                      sol::state &solState,
                                      ThreadPool &threadPool)
    : m_registry(), m_entity(createEntity()),
      m_luaState(enchanceLuaState(solState)), m_threadPool(threadPool),
      m_eventDispatcher(ed) {};

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::updateSystems(DeltaTime deltaTime)
{
  PROFILE_FUNCTION();
  flushMainThreadJobs();
  for (auto *sys : m_updateSystems)
    static_cast<IOnUpdate *>(sys)->onUpdate(deltaTime);
  m_eventDispatcher.update();
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::updateSystems(const SDL_Event &event)
{
  PROFILE_SCOPE("Scene::updateSystems - updates for native scripts");
  for (auto *sys : m_eventSystems)
    static_cast<IOnEvent *>(sys)->onEvent(event);
}
template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::renderSystems(RenderPass pass,
                                           RenderApi &renderers,
                                           bool isMinimized,
                                           DeltaTime currentTime)
{
  PROFILE_SCOPE("Scene::renderSystems - rendering native scripts");
  for (auto *sys : m_renderSystems[static_cast<uint8_t>(pass)])
    static_cast<IOnRender *>(sys)->onRender(renderers, isMinimized,
                                            currentTime);
}

template class AbstractScene<WorldComponents>;
template class AbstractScene<UIComponents>;

} // namespace pain
