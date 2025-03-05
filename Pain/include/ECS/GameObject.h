#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Scene.h"
#include <utility>

namespace pain
{

class GameObject
{
public:
  GameObject(Scene *scene);
  ~GameObject();
  template <typename... Components>
  std::tuple<Components &...> addComponents(Components &&...args)
  {
    return m_scene->addComponents<Components...>(
        m_entity, std::forward<Components>(args)...);
  }
  template <typename T, typename... Args> T &addComponent(Args &&...args)
  {
    return m_scene->addComponent<T>(m_entity, std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  template <typename... Components>
  std::tuple<Components &...> getAllComponents()
  {
    return m_scene->getAllComponents<Components...>(m_entity);
  }
  template <typename... Components>
  const std::tuple<Components &...> getAllComponents() const
  {
    return static_cast<const Scene *>(m_scene)->getAllComponents<Components...>(
        m_entity);
  }
  template <typename T, typename... Components>
    requires IsNoneType<Components...>
  T &getComponent()
  {
    return m_scene->getComponent<T, T>(m_entity);
  }
  template <typename T, typename... Components>
    requires IsNoneType<Components...>
  const T &getComponent() const
  {
    return static_cast<const Scene *>(m_scene)->getComponent<T, T>(m_entity);
  }
  template <typename T, typename... Components>
    requires(!IsNoneType<Components...>)
  T &getComponent()
  {
    return m_scene->getComponent<T, Components...>(m_entity);
  }
  template <typename T, typename... Components>
    requires(!IsNoneType<Components...>)
  const T &getComponent() const
  {
    return static_cast<const Scene *>(m_scene)->getComponent<T, Components...>(
        m_entity);
  }

  // ---------------------------------------------------- //
  // Does archetype has components?
  // ---------------------------------------------------- //
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
