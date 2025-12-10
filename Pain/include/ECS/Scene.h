// Scene.h
#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"
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
  void insertColliders(const std::vector<reg::Entity> &entities);

  // ---------------------------------------------------- //
  // Constructors
  // ---------------------------------------------------- //

  Scene(sol::state &solState, void *context, SDL_Window *window);

  // ---------------------------------------------------- //
  // Helper function to "emplace" Scripts
  // ---------------------------------------------------- //

  // Move already made script to the registry
  template <typename T> T &emplaceScript(T &&t)
  {
    NativeScriptComponent &nsc =
        m_registry.getComponent<NativeScriptComponent>(m_entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  // Emplace script inside the registry
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  T &emplaceScript(Args &&...args)
  {
    NativeScriptComponent &nsc =
        m_registry.getComponent<NativeScriptComponent>(m_entity);
    nsc.bindAndInitiate<T>(m_entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  // Move already made script to the registry
  template <typename T> T &emplaceImGuiScript(T &&t)
  {
    ImGuiComponent &nsc = m_registry.getComponent<ImGuiComponent>(m_entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  T &emplaceImGuiScript(Args &&...args)
  {
    ImGuiComponent &nsc = m_registry.getComponent<ImGuiComponent>(m_entity);
    nsc.bindAndInitiate<T>(m_entity, *this, std::forward<Args>(args)...);
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

  template <typename T>
    requires reg::IsSingleType<T>
  inline reg::Iterator<T> end()
  {
    return m_registry.template end<T>();
  }
  template <typename T>
    requires reg::IsSingleType<T>
  inline reg::Iterator<T> begin()
  {
    return m_registry.template begin<T>();
  }
  template <typename... Components, typename... ExcludeComponents>
    requires reg::IsMultipleTypes<Components...>
  inline std::tuple<reg::Iterator<Components>...>
  begin(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template begin<Components...>(
        exclude<ExcludeComponents...>);
  }
  template <typename... Components, typename... ExcludeComponents>
    requires reg::IsMultipleTypes<Components...>
  inline std::tuple<reg::Iterator<Components>...>
  end(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template end<Components...>(
        exclude<ExcludeComponents...>);
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
    return std::as_const(m_registry).containsAll<TargetComponents...>(entity);
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

private:
  reg::ArcheRegistry<ComponentManager> m_registry;
  sol::state &m_luaState;
  reg::Entity m_entity;
  Systems::Render *m_renderSystem;
  Systems::Kinematics *m_kinematicsSystem;
  Systems::NativeScript *m_nativeScriptSystem;
  Systems::ImGuiSys *m_imGuiSystem;
  Systems::LuaScript *m_luaSystem;
  Systems::SweepAndPruneSys *m_sweepAndPruneSystem;
};
} // namespace pain
