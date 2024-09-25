#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"

namespace pain
{

class ScriptableEntity
{
public:
  template <typename T> T &getComponent()
  {
    return m_scene->getComponent<T>(m_entity);
  }
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
