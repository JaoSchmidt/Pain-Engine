#pragma once

#include "Core.h"

#include <queue>

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Registry.h"
#include "Entity.h"

namespace pain
{
class GameObject;
struct NativeScriptComponent;

class Scene
{
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
  // template <typename... Components, typename... Args>
  // void addComponents(Entity entity, Args &&...args)
  // {
  //   // P_ASSERT_W(!hasComponent<Components>(entity), "Entity {} already has
  //   // component!",
  //   //            entity);
  //   return m_registry->add<Components...>(entity,
  //   std::forward<Args>(args)...);
  // }
  // ---------------------------------------------------- //
  // Component Stuff
  // ---------------------------------------------------- //
  template <typename T, typename... Args>
  T &addComponent(Entity entity, Args &&...args)
  {
    P_ASSERT_W(!hasComponent<T>(entity), "Entity {} already has component!",
               entity);
    return m_registry->add<T>(entity, std::forward<Args>(args)...);
  }
  template <typename T> T &getComponent(Entity entity)
  {
    return m_registry->get<T>(entity);
  }
  template <typename T> const T &getComponent(Entity entity) const
  {
    return m_registry->get<T>(entity);
  }
  template <typename T> bool hasComponent(Entity entity)
  {
    return m_registry->has<T>(entity);
  }
  template <typename T> void rmComponent(Entity entity)
  {
    m_registry->remove<T>(entity);
  }
  template <typename T> std::unordered_map<Entity, T>::iterator begin()
  {
    return m_registry->getComponentMap<T>().begin();
  }
  template <typename T> std::unordered_map<Entity, T>::iterator end()
  {
    return m_registry->getComponentMap<T>().end();
  }
  template <typename T>
  const std::unordered_map<Entity, T>::const_iterator begin() const
  {
    return m_registry->getComponentMap<T>().begin();
  }
  template <typename T>
  const std::unordered_map<Entity, T>::const_iterator end() const
  {
    return m_registry->getComponentMap<T>().end();
  }

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
  Registry *m_registry;
  inline static std::queue<Entity> m_availableEntities = {};
  inline static Entity numberOfEntities = 0;
};
} // namespace pain
