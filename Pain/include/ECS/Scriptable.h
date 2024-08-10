#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/GameObject.h"

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

protected:
  Scene *m_scene = nullptr;
  Entity m_entity = 0;
  friend class Scene;
};
} // namespace pain
