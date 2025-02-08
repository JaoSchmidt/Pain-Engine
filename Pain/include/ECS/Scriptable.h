#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"

namespace pain
{

class ScriptableEntity
{
public:
  // return the components of an entity, as a tuple
  template <typename... Components> Tuple<Components &...> &getComponents()
  {
    return m_scene->getComponents<Components...>(m_entity);
  }
  // return the components of an entity, as a tuple
  template <typename... Components>
  const Tuple<Components &...> &getComponents() const
  {
    return m_scene->getComponents<Components...>(m_entity);
  }

  // return a single component of an entity
  template <typename T> T &getComponent()
  {
    return m_scene->getComponent<T>(m_entity);
  }
  // return a single component of an entity
  template <typename T> const T &getComponent() const
  {
    return m_scene->getComponent<T>(m_entity);
  }

protected:
  Entity m_entity = 0;
  Scene *m_scene = nullptr;
  friend class Scene;
};
} // namespace pain
