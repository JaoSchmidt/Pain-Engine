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
  ~Scene();

  Entity createEntity();
  void destroyEntity(Entity entity);

  void onUpdate(DeltaTime ts);

  template <typename T, typename... Args>
  T &addComponent(Entity entity, Args &&...args)
  {
    P_ASSERT_W(!hasComponent<T>(entity), "Entity already has component!");
    return m_registry.add<T>(entity, std::forward<Args>(args)...);
  }

  template <typename T> T &getComponent(Entity entity)
  {
    return m_registry.get<T>(entity);
  }

  template <typename T> bool hasComponent(Entity entity)
  {
    return m_registry.has<T>(entity);
  }

  template <typename T> void rmComponent(Entity entity)
  {
    m_registry.remove<T>(entity);
  }
  template <typename T> std::unordered_map<Entity, T> &getComponentMap()
  {
    return m_registry.getComponentMap<T>();
  }

private:
  Registry m_registry;
  static std::queue<Entity> m_availableEntities;
  static Entity numberOfEntities;
};

} // namespace pain
