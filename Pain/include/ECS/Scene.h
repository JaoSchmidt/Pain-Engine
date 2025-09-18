#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/System.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/WithScript.h"
#include "Entity.h"
#include "GUI/ImGuiSystem.h"
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
  ArcheRegistry m_registry;
  sol::state &m_luaState;
  Entity m_entity;
  Bitmask m_bitmask = 0;

public:
  template <typename... Components>
  Scene(void *context, SDL_Window *window, sol::state &solState)
      : m_registry(), m_luaState(solState), m_entity(createEntity()),
        m_imGuiSystem(m_registry, context, window){};
  template <typename... Components>
  Scene(void *context, SDL_Window *window, sol::state &luaState,
        Components &&...args)
      : m_registry(), m_luaState(luaState), m_entity(createEntity()),
        m_imGuiSystem(m_registry, context, window)
  {
    createComponents<Components...>(m_entity,
                                    std::forward<Components>(args)...);
    m_bitmask = ComponentManager::multiComponentBitmask<Components...>();
  };
  Entity createEntity();
  void destroyEntity(Entity entity);

  template <typename T, typename... Args> void withScript(Args &&...args)
  {
    SceneHelper::withScript<T>(m_entity, m_registry, *this,
                               std::forward<Args>(args)...);
  }

  template <typename T, typename... Args> void withImGuiScript(Args &&...args)
  {
    SceneHelper::withImGuiScript<T>(m_entity, m_registry, *this,
                                    std::forward<Args>(args)...);
  }

  sol::state &getSharedLuaState() { return m_luaState; }
  // ---------------------------------------------------- //
  // Component Archetype stuff
  // ---------------------------------------------------- //
  template <typename... Components, typename... Args>
  std::tuple<Components &...> createComponents(Entity entity, Args &&...args)
  {
    return m_registry.createComponents<Components...>(
        entity, std::forward<Args>(args)...);
  }
  template <typename T, typename... Args>
  T &createComponent(Entity entity, Args &&...args)
  {
    return m_registry.createComponent<T>(entity, std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  // get multiple components together as a tuple
  template <typename... Components>
  std::tuple<Components &...> getComponents(Entity entity, Bitmask bitmask)
  {
    return m_registry.getComponents<Components...>(entity, bitmask);
  }
  // get multiple components together as a tuple
  template <typename... Components>
  const std::tuple<const Components &...> getComponents(Entity entity,
                                                        Bitmask bitmask) const
  {
    return std::as_const(m_registry)
        .getComponents<Components...>(entity, bitmask);
  }
  // get a single component
  template <typename T> T &getComponent(Entity entity, Bitmask bitmask)
  {
    return m_registry.getComponent<T>(entity, bitmask);
  }
  // get a single component
  template <typename T>
  const T &getComponent(Entity entity, Bitmask bitmask) const
  {
    return std::as_const(m_registry).getComponent<T>(entity, bitmask);
  }
  // ---------------------------------------------------- //
  // "Has" functions
  // ---------------------------------------------------- //
  template <typename... TargetComponents>
  constexpr bool hasAnyComponents(int bitmask) const
  {
    return std::as_const(m_registry).hasAny<TargetComponents...>(bitmask);
  }
  template <typename... TargetComponents>
  constexpr bool containsAllComponents(int bitmask) const
  {
    return std::as_const(m_registry).containsAll<TargetComponents...>(bitmask);
  }
  // remove an entity, alongside its components from it's archetype
  bool removeEntity(Entity entity, int bitmask)
  {
    if (m_registry.remove(entity, bitmask)) {
      destroyEntity(entity);
      return true;
    } else
      return false;
  }
  // Remove components of an entity
  // template <typename... TargetComponents, typename... ObjectComponents>
  // void rmComponent(Entity entity)
  // {
  //   m_registry->remove<TargetComponents..., ObjectComponents...>(entity);
  // }

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> end()
  {
    return m_registry.end<T>();
  }
  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> begin()
  {
    return m_registry.begin<T>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> begin()
  {
    return m_registry.begin<Components...>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
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
  static void clearQueue()
  {
    std::queue<Entity> empty;
    std::swap(m_availableEntities, empty);
  }

  template <typename... Args> void renderSystems(Args &&...args)
  {
    m_renderSystem.onRender(std::forward<Args>(args)...);
    m_nativeScriptSystem.onRender(std::forward<Args>(args)...);
    m_imGuiSystem.onRender(std::forward<Args>(args)...);
    m_luaSystem.onRender(std::forward<Args>(args)...);
  }

private:
  inline static std::queue<Entity> m_availableEntities = {};
  inline static Entity numberOfEntities = 0;
  Systems::Render m_renderSystem = {m_registry};
  Systems::Kinematics m_kinematicsSystem = {m_registry};
  Systems::NativeScript m_nativeScriptSystem = {m_registry};
  Systems::ImGui m_imGuiSystem;
  Systems::LuaScript m_luaSystem = {m_registry};
};
} // namespace pain
