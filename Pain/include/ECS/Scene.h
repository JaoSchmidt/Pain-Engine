#pragma once

#include "Core.h"

#include <queue>
#include <utility>

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "Entity.h"
#include <sol/sol.hpp>

namespace pain
{

class Scene
{
private:
  ArcheRegistry m_registry;
  sol::state &m_luaState;

public:
  Scene(sol::state &solState);
  void initializeScript(Scene *scene, NativeScriptComponent &nsc, Entity entity,
                        Bitmask archetype);

  Entity createEntity();
  void destroyEntity(Entity entity);

  virtual ~Scene() = default;
  virtual void onRender(double realTime) = 0;
  virtual void onUpdate(double deltaTime) = 0;
  virtual void onEvent(const SDL_Event &event) = 0;

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
  void renderSystems(double currentTime);
  static void clearQueue()
  {
    std::queue<Entity> empty;
    std::swap(m_availableEntities, empty);
  }

private:
  inline static std::queue<Entity> m_availableEntities = {};
  inline static Entity numberOfEntities = 0;
};
} // namespace pain
