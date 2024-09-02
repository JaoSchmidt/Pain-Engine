#pragma once

#include "Core.h"
#include "ECS/Entity.h"

namespace pain
{

class EXPORT ScriptableEntity
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

private:
  Entity m_entity = 0;
  Scene *m_scene = nullptr;
  friend class Scene;
};
} // namespace pain
