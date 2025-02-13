#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"
#include <utility>

namespace pain
{

class GameObject
{
public:
  GameObject(Scene *scene);
  ~GameObject();
  template <typename... Components, typename... Args>
  Tuple<Components &...> addComponents(Args &&...args)
  {
    return m_scene->addComponents<Components...>(m_entity,
                                                 std::forward<Args>(args)...);
  }
  template <typename T, typename... Args> T &addComponent(Args &&...args)
  {
    return m_scene->addComponent<T>(m_entity, std::forward<Args>(args)...);
  }
  template <typename... Components> Tuple<Components &...> getComponents()
  {
    return m_scene->getComponents<Components...>(m_entity);
  }
  template <typename... Components>
  const Tuple<Components &...> getComponents() const
  {
    return m_scene->getComponents<Components...>(m_entity);
  }
  template <typename T> T &getComponent()
  {
    return m_scene->getComponent<T>(m_entity);
  }
  template <typename T> const T &getComponent() const
  {
    return m_scene->getComponent<T>(m_entity);
  }

  template <typename... Components> bool hasComponent(Entity entity) const
  {
    return m_scene->hasComponent<Components...>(entity);
  }
  template <typename... Components> void rmComponent(Entity entity)
  {
    m_scene->rmComponent<Components...>(entity);
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
