// Scene.h
#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "ECS/EventDispatcher.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Entity.h"
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

class Scene
{

public:
  reg::ArcheRegistry<ComponentManager> &getRegistry() { return m_registry; };
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

  Scene(sol::state &solState, void *context, SDL_Window *window);

  // ---------------------------------------------------- //
  // Helper function to "emplace" Scripts
  // ---------------------------------------------------- //
  void emplaceLuaScript(reg::Entity entity, const char *scriptPath);

  // Move already made script to the registry
  template <typename T> T &emplaceScript(reg::Entity entity, T &&t)
  {
    NativeScriptComponent &nsc =
        m_registry.getComponent<NativeScriptComponent>(entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  // Emplace script inside the registry
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  T &emplaceScript(reg::Entity entity, Args &&...args)
  {
    NativeScriptComponent &nsc =
        m_registry.getComponent<NativeScriptComponent>(entity);
    nsc.bindAndInitiate<T>(entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  // Move already made script to the registry
  template <typename T> T &emplaceImGuiScript(reg::Entity entity, T &&t)
  {
    ImGuiComponent &nsc = m_registry.getComponent<ImGuiComponent>(entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  T &emplaceImGuiScript(reg::Entity entity, Args &&...args)
  {
    ImGuiComponent &nsc = m_registry.getComponent<ImGuiComponent>(entity);
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
    return m_registry.createComponents<Components...>(
        entity, std::forward<Components>(components)...);
  }
  template <typename T, typename... Args>
  T &createComponent(reg::Entity entity, Args &&...args)
  {
    return m_registry.createComponent<T>(entity, std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.query<Components...>(exclude<ExcludeComponents...>);
  }
  template <typename... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<const Components...>>
  queryConst(exclude_t<ExcludeComponents...> = {}) const
  {
    return m_registry.queryConst<Components...>(exclude<ExcludeComponents...>);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //

  // get multiple components together as a tuple
  template <typename... Components>
  std::tuple<Components &...> getComponents(reg::Entity entity)
  {
    return m_registry.getComponents<Components...>(entity);
  }
  // get multiple components together as a tuple
  template <typename... Components>
  const std::tuple<const Components &...>
  getComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry).getComponents<Components...>(entity);
  }
  // get a single component
  template <typename T> T &getComponent(reg::Entity entity)
  {
    return m_registry.getComponent<T>(entity);
  }
  // get a single component
  template <typename T> const T &getComponent(reg::Entity entity) const
  {
    return std::as_const(m_registry).getComponent<T>(entity);
  }
  // ---------------------------------------------------- //
  // "Has" functions
  // ---------------------------------------------------- //
  template <typename... TargetComponents>
  bool hasAnyComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry).hasAny<TargetComponents...>(entity);
  }
  template <typename... TargetComponents>
  constexpr bool containsAllComponents(reg::Entity entity) const
  {
    return m_registry.containsAll<TargetComponents...>(entity);
  }
  // remove an entity, alongside its components from it's archetype,
  // NOTE: The caller needs to tell me the components of the entity
  template <typename... Components> bool removeEntity(reg::Entity entity)
  {
    return m_registry.remove<Components...>(entity);
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
  Systems::SweepAndPruneSys *getCollisionSys() { return m_sweepAndPruneSystem; }

  reg::EventDispatcher &getEventDispatcher() { return m_eventDispatcher; }
  const reg::EventDispatcher &getEventDispatcher() const
  {
    return m_eventDispatcher;
  }
  ~Scene();

private:
  reg::ArcheRegistry<ComponentManager> m_registry;
  reg::EventDispatcher m_eventDispatcher;
  sol::state &m_luaState;
  reg::Entity m_entity;
  Systems::Render *m_renderSystem;
  Systems::Kinematics *m_kinematicsSystem;
  Systems::NativeScript *m_nativeScriptSystem;
  Systems::ImGuiSys *m_imGuiSystem;
  Systems::LuaScript *m_luaSystem;
  Systems::SweepAndPruneSys *m_sweepAndPruneSystem;
  Systems::CameraSys *m_cameraSystem;
};
} // namespace pain
