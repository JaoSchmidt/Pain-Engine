/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

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

/**
 * @brief Generic scene abstraction owning an ECS registry, systems, scripting,
 *        threading utilities, and engine event dispatching.
 *
 * AbstractScene represents a high-level execution context of the engine.
 * It owns:
 *  - An archetype registry for entity/component storage.
 *  - A set of systems responsible for update, rendering and event processing.
 *  It references from the application:
 *  - A shared Lua state for gameplay and tooling scripting.
 *  - A thread pool with a main-thread job queue.
 *  - An event dispatcher for engine events.
 *
 * The scene is parameterized by a compile-time component manager, allowing
 * multiple specialized scene types (e.g. world scene, UI scene) to coexist
 * with distinct component sets and registries.
 *
 * @tparam Manager Compile-time component manager defining which ECS components
 *                 are registered and available in this scene.
 */
template <reg::CompileTimeBitMaskType Manager> class AbstractScene
{
public:
  // =============================================================== //
  // REGISTRY RELATED
  // =============================================================== //

  /// @brief Returns the underlying archetype registry.
  reg::ArcheRegistry<Manager> &getRegistry() { return m_registry; }

  /// @brief Returns the root entity associated with this scene.
  reg::Entity getEntity() const { return m_entity; }

  /**
   * @brief Creates a new entity in the registry.
   *
   * @return Newly created entity identifier.
   */
  inline reg::Entity createEntity() { return m_registry.createEntity(); }

  /**
   * @brief Creates multiple components at once for an entity.
   *
   * All components are moved into the registry and a single archetype
   * transition is performed when possible.
   *
   * @tparam Components ECS component types.
   * @param entity Target entity.
   * @param components Component instances to move.
   * @return Tuple of references to the created components.
   */
  template <reg::ECSComponent... Components>
  std::tuple<Components &...> createComponents(reg::Entity entity,
                                               Components &&...components)
  {
    return m_registry.template createComponents<Components...>(
        entity, std::forward<Components>(components)...);
  }

  /**
   * @brief Creates a single component on an entity.
   *
   * @tparam T Component type.
   * @param entity Target entity.
   * @param args Constructor arguments for the component.
   * @return Reference to the created component.
   */
  template <reg::ECSComponent T, typename... Args>
  T &createComponent(reg::Entity entity, Args &&...args)
  {
    return m_registry.template createComponent<T>(entity,
                                                  std::forward<Args>(args)...);
  }

  /**
   * @brief Adds multiple components to an existing entity.
   *
   * @tparam Components ECS component types.
   * @param entity Target entity.
   * @param components Component instances to add.
   * @return Tuple of references to the added components.
   */
  template <reg::ECSComponent... Components>
  std::tuple<Components &...> addComponents(reg::Entity entity,
                                            Components &&...components)
  {
    return m_registry.template addComponents<Components...>(
        entity, std::forward<Components>(components)...);
  }

  /**
   * @brief Returns the bitmask corresponding to a single component type.
   *
   * @tparam Component Component type.
   */
  template <reg::ECSComponent Component> reg::Bitmask getSingleBitmask()
  {
    return Manager::template singleComponentBitmask<Component>();
  }

  /**
   * @brief Queries archetypes containing the given components.
   *
   * @tparam Components Required component types.
   * @tparam ExcludeComponents Component types to exclude.
   * @param Exclude Optional exclusion mask.
   * @return Vector of chunk views matching the query.
   */
  template <reg::ECSComponent... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template query<Components...>(
        exclude<ExcludeComponents...>);
  }

  /// @brief Const version of query().
  template <reg::ECSComponent... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<const Components...>>
  queryConst(exclude_t<ExcludeComponents...> = {}) const
  {
    return m_registry.template queryConst<Components...>(
        exclude<ExcludeComponents...>);
  }

  /**
   * @brief Retrieves multiple components from an entity.
   *
   * @tparam Components Component types.
   * @param entity Target entity.
   * @return Tuple of references to the requested components.
   */
  template <reg::ECSComponent... Components>
  std::tuple<Components &...> getComponents(reg::Entity entity)
  {
    return m_registry.template getComponents<Components...>(entity);
  }

  /// @brief Const version of getComponents().
  template <reg::ECSComponent... Components>
  const std::tuple<const Components &...>
  getComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template getComponents<Components...>(entity);
  }

  /** @brief Retrieves a single component from an entity. */
  template <reg::ECSComponent T> T &getComponent(reg::Entity entity)
  {
    return m_registry.template getComponent<T>(entity);
  }

  /** @brief Const version of getComponent(). */
  template <reg::ECSComponent T> const T &getComponent(reg::Entity entity) const
  {
    return std::as_const(m_registry).template getComponent<T>(entity);
  }

  /** @brief Checks if an entity has at least one of the given components. */
  template <reg::ECSComponent... TargetComponents>
  bool hasAnyComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template hasAny<TargetComponents...>(entity);
  }

  /** @brief Checks if an entity contains all specified components. */
  template <reg::ECSComponent... TargetComponents>
  constexpr bool containsAllComponents(reg::Entity entity) const
  {
    return m_registry.template containsAll<TargetComponents...>(entity);
  }

  /** @brief Removes an entity and all of its components from the registry. */
  void removeEntity(reg::Entity entity) { m_registry.remove(entity); }

  // =============================================================== //
  // LUA SCRIPTING RELATED
  // =============================================================== //

  /**
   * @brief Attaches and initializes a Lua script component on an entity.
   *
   * Only available when the LuaScriptComponent is registered by the manager.
   *
   * @param entity Target entity.
   * @param scriptPath Path to the script file.
   */
  void emplaceLuaScript(reg::Entity entity, const char *scriptPath)
    requires(Manager::template isRegistered<tag::LuaScript>());

  /** @brief Returns the shared Lua state used by the scene. */
  sol::state &getSharedLuaState() { return m_luaState; }

  /**
   * @brief Registers entity creation and component binding helpers in Lua.
   *
   * @param sceneName Name exposed to the Lua environment.
   * @param lua Lua state to bind into.
   */
  void addEntityFunctions(const char *sceneName, sol::state &lua);

  // =============================================================== //
  // SYSTEMS RELATED
  // =============================================================== //

  /** @brief Updates all systems implementing IOnUpdate. */
  void updateSystems(DeltaTime deltaTime);

  /** @brief Dispatches input events to systems implementing IOnEvent. */
  void updateSystems(const SDL_Event &event);

  /** @brief Executes render callbacks on systems implementing IOnRender. */
  void renderSystems(Renderer2d &renderer, bool isMinimized,
                     DeltaTime currentTime);

  /**
   * @brief Retrieves a system by its concrete type.
   *
   * @tparam Sys System type.
   * @return Pointer to the system if found, otherwise nullptr.
   */
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

  // =============================================================== //
  // MULTI THREAD RELATED
  // =============================================================== //

  /** @brief Returns the thread pool associated with the scene. */
  ThreadPool &getThreadPool() { return m_threadPool; }

  /** @brief Const access to the thread pool. */
  const ThreadPool &getThreadPool() const { return m_threadPool; }

  using MainThreadJob = std::function<void()>;

  /**
   * @brief Enqueues a job to be executed on the main thread.
   *
   * Jobs are executed during flushMainThreadJobs().
   */
  void enqueueMainThread(MainThreadJob job);

  /**
   * @brief Executes and clears all queued main-thread jobs.
   *
   * Intended to be called once per frame.
   */
  void flushMainThreadJobs();

  // =============================================================== //
  // ENGINE EVENTS RELATED
  // =============================================================== //

  /** @brief Returns the event dispatcher associated with the scene. */
  reg::EventDispatcher &getEventDispatcher() { return m_eventDispatcher; }

  /** @brief Const access to the event dispatcher. */
  const reg::EventDispatcher &getEventDispatcher() const
  {
    return m_eventDispatcher;
  }

  // =============================================================== //
  // CONSTRUCTION
  // =============================================================== //

  /**
   * @brief Constructs a scene bound to an event dispatcher, Lua state,
   *        and thread pool.
   */
  AbstractScene(reg::EventDispatcher &ed, sol::state &solState,
                ThreadPool &threadPool);

  AbstractScene() = delete;

protected:
  /// ECS registry storing entities and components.
  reg::ArcheRegistry<Manager> m_registry;

  /// Root entity for the scene.
  reg::Entity m_entity;

  /// Allows derived scenes to extend the Lua environment.
  sol::state &enchanceLuaState(sol::state &state);

  /// Shared Lua state reference.
  sol::state &m_luaState;

  /// All registered systems indexed by type.
  std::map<std::type_index, std::unique_ptr<System<Manager>>> m_systems;

  /// Cached system lists for fast iteration.
  std::vector<IOnUpdate *> m_updateSystems;
  std::vector<IOnEvent *> m_eventSystems;
  std::vector<IOnRender *> m_renderSystems;

  /// Thread pool used by the scene.
  ThreadPool &m_threadPool;

  /// Synchronization for main-thread job queue.
  std::mutex m_mainThreadMutex;

  /// Pending jobs executed on the main thread.
  std::queue<MainThreadJob> m_mainThreadJobs;

  /// Event dispatcher used by the scene.
  reg::EventDispatcher &m_eventDispatcher;
};

} // namespace pain
