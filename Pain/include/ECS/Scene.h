#pragma once

#include "Core.h"

#include <queue>

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/Registry.h"
#include "Entity.h"

namespace pain
{
class GameObject;
struct NativeScriptComponent;

template <typename... Ts>
concept IsSingleType = (sizeof...(Ts) == 1);

template <typename... Ts>
concept IsMultipleTypes = (sizeof...(Ts) > 1);

class Scene
{
private:
  ArcheRegistry<Archetype<MovementComponent, TransformComponent>> *m_registry =
      {};

public:
  Scene();
  ~Scene() { delete m_registry; };

  Entity createEntity();
  void destroyEntity(Entity entity);

  virtual void onRender(double realTime) = 0;
  virtual void onUpdate(double deltaTime) = 0;
  virtual void onEvent(const SDL_Event &event) = 0;

  // ---------------------------------------------------- //
  // Component Archetype stuff
  // ---------------------------------------------------- //
  template <typename... Components, typename... Args>
  Tuple<Components &...> addComponents(Entity entity, Args &&...args)
  {
    // P_ASSERT_W(!hasComponent<Components>(entity), "Entity {} already has
    // component!",
    //            entity);
    return m_registry->addComponents<Components...>(
        entity, std::forward<Args>(args)...);
  }
  template <typename T, typename... Args>
  T &addComponent(Entity entity, Args &&...args)
  {
    return m_registry->addComponent<T>(entity, std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  // get multiple components together as a tuple
  template <typename... Components>
  Tuple<Components &...> &getComponents(Entity entity)
  {
    return m_registry->getComponents<Components...>(entity);
  }
  // get multiple components together as a tuple
  template <typename... Components>
  const Tuple<const Components &...> &getComponents(Entity entity) const
  {
    return m_registry->getComponents<Components...>(entity);
  }

  // get a single component
  template <typename T> T &getComponent(Entity entity)
  {
    return m_registry->getComponent<T>(entity);
  }
  // get a single component
  template <typename T> const T &getComponent(Entity entity) const
  {
    return m_registry->getComponent<T>(entity);
  }

  template <typename... Components> bool hasComponent(Entity entity) const
  {
    return m_registry->has<Components...>(entity);
  }
  template <typename... Components> void rmComponent(Entity entity)
  {
    m_registry->remove<Components...>(entity);
  }

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename T>
    requires IsSingleType<T>
  const reg::Iterator<T> end() const
  {
    return m_registry->end<T>();
  }
  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> begin()
  {
    return m_registry->begin<T>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  Tuple<reg::Iterator<Components>...> begin()
  {
    return m_registry->begin<Components...>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  Tuple<const reg::Iterator<Components>...> end() const
  {
    return m_registry->end<Components...>();
  }

  // ---------------------------------------------------- //
  // Component Stuff
  // ---------------------------------------------------- //
  // template <typename T, typename... Args>
  // T &addComponent(Entity entity, Args &&...args)
  // {
  //   P_ASSERT_W(!hasComponent<T>(entity), "Entity {} already has component!",
  //              entity);
  //   return m_registry->add<T>(entity, std::forward<Args>(args)...);
  // }
  // template <typename T> T &getComponent(Entity entity)
  // {
  //   return m_registry->get<T>(entity);
  // }
  // template <typename T> const T &getComponent(Entity entity) const
  // {
  //   return m_registry->get<T>(entity);
  // }
  // template <typename T> bool hasComponent(Entity entity)
  // {
  //   return m_registry->has<T>(entity);
  // }
  // template <typename T> void rmComponent(Entity entity)
  // {
  //   m_registry->remove<T>(entity);
  // }
  // template <typename T> std::unordered_map<Entity, T>::iterator begin()
  // {
  //   return m_registry->getComponentMap<T>().begin();
  // }
  // template <typename T> std::unordered_map<Entity, T>::iterator end()
  // {
  //   return m_registry->getComponentMap<T>().end();
  // }
  // template <typename T>
  // const std::unordered_map<Entity, T>::const_iterator begin() const
  // {
  //   return m_registry->getComponentMap<T>().begin();
  // }
  // template <typename T>
  // const std::unordered_map<Entity, T>::const_iterator end() const
  // {
  //   return m_registry->getComponentMap<T>().end();
  // }

  // ---------------------------------------------------- //
  // Systems
  // ---------------------------------------------------- //

  void updateSystems(double deltaTime);
  void updateSystems(const SDL_Event &event);
  void initializeScripts(NativeScriptComponent &nativeScriptComponent,
                         const GameObject &go);
  void initializeScripts(NativeScriptComponent &nativeScriptComponent,
                         Entity entity);
  void renderSystems(double currentTime);

private:
  inline static std::queue<Entity> m_availableEntities = {};
  inline static Entity numberOfEntities = 0;
};
} // namespace pain
