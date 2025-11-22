// Scene.h
#pragma once

#include "Core.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Entity.h"
#include "GUI/ImGuiComponent.h"
#include "Scripting/NativeSystem.h"

#include <sol/sol.hpp>
#include <utility>

namespace pain
{
class Scene
{
private:
  reg::ArcheRegistry<ComponentManager> m_registry;
  sol::state &m_luaState;
  reg::Entity m_entity;

public:
  reg::ArcheRegistry<ComponentManager> &getRegistry() { return m_registry; };
  reg::Entity getEntity() const { return m_entity; }
  void insertStaticCollider(reg::Entity entity);
  sol::state &getSharedLuaState() { return m_luaState; }
  inline reg::Entity createEntity() { return m_registry.createEntity(); }
  // ---------------------------------------------------- //
  // Constructors
  // ---------------------------------------------------- //
  template <typename... Components>
  Scene(sol::state &solState)
      : m_registry(), m_luaState(solState), m_entity(createEntity()){};
  template <typename... Components>
  Scene(sol::state &luaState, Components &&...args)
      : m_registry(), m_luaState(luaState), m_entity(createEntity())
  {
    createComponents<Components...>(m_entity,
                                    std::forward<Components>(args)...);
  };
  // ---------------------------------------------------- //
  // Scripts
  // ---------------------------------------------------- //
  template <typename T> void emplaceScript(T &&t)
  {
    NativeScriptComponent &nsc =
        m_registry.getComponent<NativeScriptComponent>(m_entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  void emplaceScript(Args &&...args)
  {
    NativeScriptComponent &nsc =
        m_registry.getComponent<NativeScriptComponent>(m_entity);
    nsc.bindAndInitiate<T>(m_entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }

  template <typename T> void emplaceImGuiScript(T &&t)
  {
    ImGuiComponent &nsc = m_registry.getComponent<ImGuiComponent>(m_entity);
    nsc.bindAndInitiate<T>(std::move(t));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  void emplaceImGuiScript(Args &&...args)
  {
    ImGuiComponent &nsc = m_registry.getComponent<ImGuiComponent>(m_entity);
    nsc.bindAndInitiate<T>(m_entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }

  // ---------------------------------------------------- //
  // Component Archetype stuff
  // ---------------------------------------------------- //
  template <typename... Components, typename... Args>
  std::tuple<Components &...> createComponents(reg::Entity entity,
                                               Args &&...args)
  {
    return m_registry.createComponents<Components...>(
        entity, std::forward<Args>(args)...);
  }
  template <typename T, typename... Args>
  T &createComponent(reg::Entity entity, Args &&...args)
  {
    return m_registry.createComponent<T>(entity, std::forward<Args>(args)...);
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

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename T>
    requires reg::IsSingleType<T>
  reg::Iterator<T> end()
  {
    return m_registry.end<T>();
  }
  template <typename T>
    requires reg::IsSingleType<T>
  reg::Iterator<T> begin()
  {
    return m_registry.begin<T>();
  }
  template <typename... Components>
    requires reg::IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> begin()
  {
    return m_registry.begin<Components...>();
  }
  template <typename... Components>
    requires reg::IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> end()
  {
    return m_registry.end<Components...>();
  }
};
} // namespace pain
