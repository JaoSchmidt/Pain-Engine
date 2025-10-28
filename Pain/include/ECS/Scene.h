#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/System.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Entity.h"
#include "ECS/WithScript.h"
#include "GUI/ImGuiSystem.h"
#include "Physics/Collision/CollisionSystem.h"
#include "Physics/Kinematics.h"
#include "Scripting/LuaScriptSystem.h"
#include "Scripting/NativeSystem.h"

#include <queue>
#include <sol/sol.hpp>
#include <utility>

namespace pain
{
class Scene
{
private:
  reg::ArcheRegistry<ComponentManager> m_registry;
  sol::state &m_luaState;
  std::string m_name = "NULL scene name";

public:
  reg::Entity m_entity;

  void insertStaticCollider(reg::Entity entity);
  template <typename... Components>
  Scene(std::string &name, void *context, SDL_Window *window,
        sol::state &solState)
      : m_registry(), m_luaState(solState), m_name(name),
        m_entity(createEntity()), m_imGuiSystem(m_registry, context, window){};
  template <typename... Components>
  Scene(std::string &name, void *context, SDL_Window *window,
        sol::state &luaState, Components &&...args)
      : m_registry(), m_luaState(luaState), m_name(name),
        m_entity(createEntity()), m_imGuiSystem(m_registry, context, window)
  {
    createComponents<Components...>(m_entity,
                                    std::forward<Components>(args)...);
  };
  inline reg::Entity createEntity() { return m_registry.createEntity(); }
  template <typename T, typename... Args> void withScript(Args &&...args)
  {
    // SceneHelper::withScript<T>(m_entity, m_registry, *this,
    //                            std::forward<Args>(args)...);
    NativeScriptComponent &nsc =
        m_registry.getComponent<NativeScriptComponent>(m_entity);
    nsc.bindAndInitiate<T>(m_entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }

  template <typename T, typename... Args> void withImGuiScript(Args &&...args)
  {
    ImGuiComponent &nsc = m_registry.getComponent<ImGuiComponent>(m_entity);
    nsc.bindAndInitiate<T>(m_entity, *this, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    // SceneHelper::withImGuiScript<T>(m_entity, m_registry, *this,
    //                                 std::forward<Args>(args)...);
  }

  sol::state &getSharedLuaState() { return m_luaState; }
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

  // ---------------------------------------------------- //
  // Systems
  // ---------------------------------------------------- //

  void updateSystems(double deltaTime);
  void updateSystems(const SDL_Event &event);
  // template <typename... Args> void renderSystems(Args... args);

  template <typename... Args> void renderSystems(Args &&...args)
  {
    m_renderSystem.onRender(std::forward<Args>(args)...);
    m_nativeScriptSystem.onRender(std::forward<Args>(args)...);
    m_imGuiSystem.onRender(std::forward<Args>(args)...);
    m_luaSystem.onRender(std::forward<Args>(args)...);
  }

private:
  Systems::Render m_renderSystem = {m_registry};
  Systems::Kinematics m_kinematicsSystem = {m_registry};
  Systems::NativeScript m_nativeScriptSystem = {m_registry};
  Systems::ImGui m_imGuiSystem;
  Systems::LuaScript m_luaSystem = {m_registry};
  Systems::CollisionSystem m_collisionSystem = {16.f, m_registry};
};
} // namespace pain
