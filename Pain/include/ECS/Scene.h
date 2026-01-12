// Scene.h
#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "CoreFiles/ThreadPool.h"
#include "ECS/EventDispatcher.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Bitmask.h"
#include "ECS/Registry/Entity.h"
#include "ECS/Systems.h"
#include "Scripting/Component.h"

#include <sol/sol.hpp>
#include <utility>

namespace pain
{
namespace Systems
{
struct Render;
struct Kinematics;
struct ImGuiSys;
struct LuaScript;
struct NaiveCollisionSys;
struct SweepAndPruneSys;
struct CameraSys;
} // namespace Systems

template <reg::CompileTimeBitMaskType Manager> class AbstractScene
{

public:
  // =============================================================== //
  // =============================================================== //
  // REGISTRY RELATED
  // =============================================================== //
  // =============================================================== //
  reg::ArcheRegistry<Manager> &getRegistry() { return m_registry; };
  reg::Entity getEntity() const { return m_entity; }
  inline reg::Entity createEntity() { return m_registry.createEntity(); }

  // ---------------------------------------------------- //
  // Create components for the Archetype
  // ---------------------------------------------------- //

  // Move multiple components at once and create a single archetype in the
  // process
  template <reg::ECSComponent... Components>
  std::tuple<Components &...> createComponents(reg::Entity entity,
                                               Components &&...components)
  {
    return m_registry.template createComponents<Components...>(
        entity, std::forward<Components>(components)...);
  }
  template <reg::ECSComponent T, typename... Args>
  T &createComponent(reg::Entity entity, Args &&...args)
  {
    return m_registry.template createComponent<T>(entity,
                                                  std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Add components
  // ---------------------------------------------------- //

  template <reg::ECSComponent... Components>
  std::tuple<Components &...> addComponents(reg::Entity entity,
                                            Components &&...components)
  {
    return m_registry.template addComponents<Components...>(
        entity, std::forward<Components>(components)...);
  }
  template <reg::ECSComponent Component> reg::Bitmask getSingleBitmask()
  {
    return Manager::template singleComponentBitmask<Component>();
  }

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <reg::ECSComponent... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template query<Components...>(
        exclude<ExcludeComponents...>);
  }
  template <reg::ECSComponent... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<const Components...>>
  queryConst(exclude_t<ExcludeComponents...> = {}) const
  {
    return m_registry.template queryConst<Components...>(
        exclude<ExcludeComponents...>);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //

  // get multiple components together as a tuple
  template <reg::ECSComponent... Components>
  std::tuple<Components &...> getComponents(reg::Entity entity)
  {
    return m_registry.template getComponents<Components...>(entity);
  }
  // get multiple components together as a tuple
  template <reg::ECSComponent... Components>
  const std::tuple<const Components &...>
  getComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template getComponents<Components...>(entity);
  }
  // get a single component
  template <reg::ECSComponent T> T &getComponent(reg::Entity entity)
  {
    return m_registry.template getComponent<T>(entity);
  }
  // get a single component
  template <reg::ECSComponent T> const T &getComponent(reg::Entity entity) const
  {
    return std::as_const(m_registry).template getComponent<T>(entity);
  }
  // ---------------------------------------------------- //
  // "Has" functions
  // ---------------------------------------------------- //
  template <reg::ECSComponent... TargetComponents>
  bool hasAnyComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template hasAny<TargetComponents...>(entity);
  }
  template <reg::ECSComponent... TargetComponents>
  constexpr bool containsAllComponents(reg::Entity entity) const
  {
    return m_registry.template containsAll<TargetComponents...>(entity);
  }
  // remove an entity, alongside its components from it's archetype,
  void removeEntity(reg::Entity entity) { m_registry.remove(entity); }

  // =============================================================== //
  // LUA SCRIPTING RELATED
  // =============================================================== //

  void emplaceLuaScript(reg::Entity entity, const char *scriptPath)
    requires(Manager::template isRegistered<LuaScriptComponent>());
  sol::state &getSharedLuaState() { return m_luaState; }
  void addEntityFunctions(const char *sceneName, sol::state &lua);

  // =============================================================== //
  // =============================================================== //
  // SYSTEMS RELATED
  // =============================================================== //
  // =============================================================== //

  void updateSystems(DeltaTime deltaTime);
  void updateSystems(const SDL_Event &event);
  void renderSystems(Renderer2d &renderer, bool isMinimized,
                     DeltaTime currentTime);

  template <typename Sys> Sys *getSys()
  {
    std::type_index key = std::type_index(typeid(Sys));
    auto it = m_systems.find(key);
    if (it != m_systems.end()) {
      return static_cast<Sys *>(it->second.get());
    }
    PLOG_E("Could not recover system {}", typeid(Sys).name());
    return nullptr;
  }

  template <typename Sys, typename... Args>
    requires std::is_constructible_v<Sys, reg::ArcheRegistry<Manager> &,
                                     reg::EventDispatcher &, Args...> &&
             std::constructible_from<Sys, reg::ArcheRegistry<Manager> &,
                                     reg::EventDispatcher &, Args...>
  void addSystem(Args &&...args)
  {
    auto [itSystem, isInserted] = m_systems.emplace(
        std::make_pair(std::type_index(typeid(Sys)), //
                       std::make_unique<Sys>(m_registry, m_eventDispatcher,
                                             std::forward<Args>(args)...) //
                       ));
    if (!isInserted) {
      PLOG_W("Could not insert System {}", typeid(Sys).name());
      return;
    }
    Sys *s = static_cast<Sys *>(itSystem->second.get());
    if constexpr (std::derived_from<Sys, IOnEvent>)
      m_eventSystems.emplace_back(s);
    if constexpr (std::derived_from<Sys, IOnRender>)
      m_renderSystems.emplace_back(s);
    if constexpr (std::derived_from<Sys, IOnUpdate>)
      m_updateSystems.emplace_back(s);
  }

  // =============================================================== //
  // =============================================================== //
  // MULTI THREAD RELATED
  // =============================================================== //
  // =============================================================== //

  ThreadPool &getThreadPool() { return m_threadPool; }
  const ThreadPool &getThreadPool() const { return m_threadPool; }
  using MainThreadJob = std::function<void()>;
  void enqueueMainThread(MainThreadJob job);
  void flushMainThreadJobs(); // call once per frame

  // =============================================================== //
  // =============================================================== //
  // ENGINE EVENTS RELATED
  // =============================================================== //
  // =============================================================== //

  reg::EventDispatcher &getEventDispatcher() { return m_eventDispatcher; }
  const reg::EventDispatcher &getEventDispatcher() const
  {
    return m_eventDispatcher;
  }

  // =============================================================== //
  // Constructors
  // =============================================================== //

  AbstractScene(reg::EventDispatcher &ed, sol::state &solState,
                ThreadPool &threadPool);
  AbstractScene() = delete;

protected:
  // Registry related
  reg::ArcheRegistry<Manager> m_registry;
  reg::Entity m_entity;

  // Lua scripting related
  sol::state &enchanceLuaState(sol::state &state);
  sol::state &m_luaState;

  // ECS Systems related
  std::map<std::type_index, std::unique_ptr<System<Manager>>> m_systems;
  std::vector<IOnUpdate *> m_updateSystems;
  std::vector<IOnEvent *> m_eventSystems;
  std::vector<IOnRender *> m_renderSystems;

  // Multi Thread Related
  ThreadPool &m_threadPool;
  std::mutex m_mainThreadMutex;
  std::queue<MainThreadJob> m_mainThreadJobs;

  // Engine Events Related
  reg::EventDispatcher &m_eventDispatcher;
};

} // namespace pain
