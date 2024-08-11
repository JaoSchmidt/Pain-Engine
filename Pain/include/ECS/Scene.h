#pragma once

#include "Core.h"

#include <queue>

#include "CoreFiles/LogWrapper.h"
#include "Entity.h"
#include "Registry.h"

namespace pain
{
class EXPORT Scene
{
public:
  Scene();
  ~Scene() { delete m_registry; };

  Entity createEntity();
  void destroyEntity(Entity entity);

  virtual void onRender() = 0;
  virtual void onUpdate(double ts) = 0;
  virtual void onEvent(const SDL_Event &event) = 0;

  // ---------------------------------------------------- //
  // Component Stuff
  // ---------------------------------------------------- //
  template <typename T, typename... Args>
  T &addComponent(Entity entity, Args &&...args)
  {
    P_ASSERT_W(!hasComponent<T>(entity), "Entity already has component!");
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

  void updateSystems(double deltaTime)
  {
    rotationSystem();
    movementSystem(deltaTime);
    scriptSystem(deltaTime);
  }
  void renderSystems() { spriteSystem(); }

  void updateSystems(const SDL_Event &event) { scriptSystem(event); }

private:
  Registry *m_registry;
  inline static std::queue<Entity> m_availableEntities = {};
  inline static Entity numberOfEntities = 0;

  void spriteSystem();
  void scriptSystem(double dt);
  void scriptSystem(const SDL_Event &event);
  void rotationSystem();
  void movementSystem(double dt);
};
} // namespace pain
