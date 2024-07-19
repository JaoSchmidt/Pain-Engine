#pragma once

#include "Core.h"

#include <queue>

#include "CoreFiles/DeltaTime.h"
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

  void updateSystems(double dt, const SDL_Event &e);
  virtual void onUpdate(double ts) = 0;
  virtual void onEvent(const SDL_Event &event) = 0;

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

  template <typename T> bool hasComponent(Entity entity)
  {
    return m_registry->has<T>(entity);
  }

  template <typename T> void rmComponent(Entity entity)
  {
    m_registry->remove<T>(entity);
  }
  template <typename T> std::unordered_map<Entity, T> &getComponentMap()
  {
    return m_registry->getComponentMap<T>();
  }

  void updateSystems(double deltaTime) { m_registry->updateSystems(deltaTime); }
  void renderSystems() { m_registry->renderSystems(); }

  void updateSystems(const SDL_Event &event)
  {
    m_registry->updateSystems(event);
  }

private:
  Registry *m_registry;
  inline static std::queue<Entity> m_availableEntities = {};
  inline static Entity numberOfEntities = 0;
};

} // namespace pain
