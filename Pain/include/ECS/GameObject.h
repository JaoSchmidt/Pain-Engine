#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"
#include <utility>

namespace pain
{

class EXPORT GameObject
{
public:
  GameObject(Scene *scene);
  ~GameObject();
  template <typename T, typename... Args> T &addComponent(Args &&...args)
  {
    return m_scene->addComponent<T, Args...>(m_entity,
                                             std::forward<Args>(args)...);
  }
  template <typename T> T &getComponent()
  {
    return m_scene->getComponent<T>(m_entity);
  }
  template <typename T> const T &getComponent() const
  {
    return m_scene->getComponent<T>(m_entity);
  }

  template <typename T> bool hasComponent()
  {
    return m_scene->hasComponent<T>(m_entity);
  }

  template <typename T> void rmComponent()
  {
    m_scene->rmComponent<T>(m_entity);
  }

  operator bool() const { return m_entity != -1; }

protected:
  template <typename arg> void initializeScript(arg &&nativeScriptComponent)
  {
    m_scene->initializeScripts(std::forward<arg>(nativeScriptComponent),
                               m_entity);
  }
  Entity m_entity = -1;
  Scene *m_scene;
  friend class Scene;
};

} // namespace pain
