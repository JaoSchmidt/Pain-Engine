// Scene.h
#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/EventDispatcher.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Bitmask.h"
#include "ECS/Registry/Entity.h"
#include "ECS/Systems.h"
#include "GUI/ImGuiComponent.h"

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
struct NativeScript;
struct NaiveCollisionSys;
struct SweepAndPruneSys;
struct CameraSys;
} // namespace Systems

struct NormalEntity;

template <reg::CompileTimeBitMaskType Manager> class AbstractScene
{

public:
  reg::ArcheRegistry<Manager> &getRegistry() { return m_registry; };
  reg::Entity getEntity() const { return m_entity; }
  sol::state &getSharedLuaState() { return m_luaState; }
  inline reg::Entity createEntity() { return m_registry.createEntity(); }

  // insert an object batch into the collider
  void insertColliders(std::initializer_list<
                       std::reference_wrapper<const std::vector<reg::Entity>>>
                           vectors);

  // ---------------------------------------------------- //
  // Constructors
  // ---------------------------------------------------- //

  static AbstractScene create(reg::EventDispatcher &eventDispatcher,
                              sol::state &solState);

  // ---------------------------------------------------- //
  // Helper function to "emplace" Scripts
  // ---------------------------------------------------- //

  void emplaceLuaScript(reg::Entity entity, const char *scriptPath)
    requires(Manager::template isRegistered<LuaScriptComponent>());

  // Move already made script to the registry
  template <typename T>
    requires(Manager::template isRegistered<NativeScriptComponent>())
  T &emplaceScript(reg::Entity entity, T &&t)
  {
    NativeScriptComponent &nsc =
        m_registry.template getComponent<NativeScriptComponent>(entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  // Emplace script inside the registry
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, AbstractScene &,
                                     Args...> &&
             (Manager::template isRegistered<NativeScriptComponent>())
  T &emplaceScript(reg::Entity entity, Args &&...args)
  {
    NativeScriptComponent &nsc =
        m_registry.template getComponent<NativeScriptComponent>(entity);
    nsc.bindAndInitiate<T>(entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  // Move already made script to the registry
  template <typename T>
    requires(Manager::template isRegistered<ImGuiComponent>())
  T &emplaceImGuiScript(reg::Entity entity, T &&t)
  {
    ImGuiComponent &nsc =
        m_registry.template getComponent<ImGuiComponent>(entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, AbstractScene &,
                                     Args...> &&
             (Manager::template isRegistered<ImGuiComponent>())
  T &emplaceImGuiScript(reg::Entity entity, Args &&...args)
  {
    ImGuiComponent &nsc =
        m_registry.template getComponent<ImGuiComponent>(entity);
    nsc.bindAndInitiate<T>(entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  // ---------------------------------------------------- //
  // Create components for the Archetype
  // ---------------------------------------------------- //

  // Move multiple components at once and create a single archetype in the
  // process
  template <typename... Components>
  std::tuple<Components &...> createComponents(reg::Entity entity,
                                               Components &&...components)
  {
    return m_registry.template createComponents<Components...>(
        entity, std::forward<Components>(components)...);
  }
  template <typename T, typename... Args>
  T &createComponent(reg::Entity entity, Args &&...args)
  {
    return m_registry.template createComponent<T>(entity,
                                                  std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Add components
  // ---------------------------------------------------- //

  template <typename... Components>
  std::tuple<Components &...> addComponents(reg::Entity entity,
                                            Components &&...components)
  {
    return m_registry.template addComponents<Components...>(
        entity, std::forward<Components>(components)...);
  }
  template <typename Component> reg::Bitmask getSingleBitmask()
  {
    return Manager::template singleComponentBitmask<Component>();
  }

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template query<Components...>(
        exclude<ExcludeComponents...>);
  }
  template <typename... Components, typename... ExcludeComponents>
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
  template <typename... Components>
  std::tuple<Components &...> getComponents(reg::Entity entity)
  {
    return m_registry.template getComponents<Components...>(entity);
  }
  // get multiple components together as a tuple
  template <typename... Components>
  const std::tuple<const Components &...>
  getComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template getComponents<Components...>(entity);
  }
  // get a single component
  template <typename T> T &getComponent(reg::Entity entity)
  {
    return m_registry.template getComponent<T>(entity);
  }
  // get a single component
  template <typename T> const T &getComponent(reg::Entity entity) const
  {
    return std::as_const(m_registry).template getComponent<T>(entity);
  }
  // ---------------------------------------------------- //
  // "Has" functions
  // ---------------------------------------------------- //
  template <typename... TargetComponents>
  bool hasAnyComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template hasAny<TargetComponents...>(entity);
  }
  template <typename... TargetComponents>
  constexpr bool containsAllComponents(reg::Entity entity) const
  {
    return m_registry.template containsAll<TargetComponents...>(entity);
  }
  // remove an entity, alongside its components from it's archetype,
  // NOTE: The caller needs to tell me the components of the entity
  template <typename... Components> bool removeEntity(reg::Entity entity)
  {
    return m_registry.template remove<Components...>(entity);
  }
  // Remove components of an entity
  // template <typename... TargetComponents, typename... ObjectComponents>
  // void rmComponent(reg::Entity entity)
  // {
  //   m_registry->remove<TargetComponents..., ObjectComponents...>(entity);
  // }

  void updateSystems(DeltaTime deltaTime);
  void updateSystems(const SDL_Event &event);
  void renderSystems(Renderer2d &renderer, bool isMinimized,
                     DeltaTime currentTime);

  // NOTE: this might need to be modified if I ever want modular systems
  // Systems::SweepAndPruneSys *getCollisionSys() { return
  // m_sweepAndPruneSystem; }

  reg::EventDispatcher &getEventDispatcher() { return m_eventDispatcher; }
  const reg::EventDispatcher &getEventDispatcher() const
  {
    return m_eventDispatcher;
  }

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

  void addEntityFunctions(const char *sceneName, sol::state &lua);

private:
  AbstractScene(reg::EventDispatcher &ed, sol::state &solState);
  sol::state &enchanceLuaState(sol::state &state);

  reg::ArcheRegistry<Manager> m_registry;
  reg::EventDispatcher &m_eventDispatcher;
  sol::state &m_luaState;
  reg::Entity m_entity;
  std::map<std::type_index, std::unique_ptr<System<Manager>>> m_systems;
  std::vector<IOnUpdate *> m_updateSystems;
  std::vector<IOnEvent *> m_eventSystems;
  std::vector<IOnRender *> m_renderSystems;
};
using Scene = AbstractScene<ComponentManager>;
using UIScene = AbstractScene<UIManager>;

} // namespace pain
