#pragma once

#include "Core.h"
#include "Debugging/Profiling.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Scene.h"
#include <utility>

namespace pain
{

// Allows easy manipulation of a game object without writing long functions
class GameObject
{
public:
  inline Entity getEntity() const { return m_entity; }
  template <typename... Components> GameObject(Scene *scene) : m_scene(scene)
  {
    m_entity = scene->createEntity();
    m_bitmask = ComponentManager::multiComponentBitmask<Components...>();
  }

  ~GameObject()
  {
    // Remove the entity, alongside its components from it's archetype
    m_scene->removeEntity(m_entity, m_bitmask);
    // Remove the entity from the queue
    m_scene->destroyEntity(m_entity);
  }
  template <typename... Components>
  std::tuple<Components &...> createComponents(Components &&...args)
  {
    return m_scene->createComponents<Components...>(
        m_entity, std::forward<Components>(args)...);
  }
  template <typename T, typename... Args> T &createComponent(Args &&...args)
  {
    return m_scene->createComponent<T>(m_entity, std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  template <typename... Components> std::tuple<Components &...> getComponents()
  {
    PROFILE_FUNCTION();
    return m_scene->getComponents<Components...>(m_entity, m_bitmask);
  }
  template <typename... Components>
  std::tuple<Components &...> getComponents() const
  {
    PROFILE_FUNCTION();
    return m_scene->getComponents<Components...>(m_entity, m_bitmask);
  }
  // ---------------------------------------------------- //
  // "Has" all components
  // ---------------------------------------------------- //
  // Does archetype has any of the target components?
  template <typename... TargetComponents> bool hasAnyComponents() const
  {
    PROFILE_FUNCTION();
    return m_scene->hasAnyComponents<TargetComponents...>(m_entity, m_bitmask);
  }

  // Does archetype has all of the target components?
  template <typename... TargetComponents> bool hasAllComponents() const
  {
    PROFILE_FUNCTION();
    return m_scene->containsAllComponents<TargetComponents...>(m_entity,
                                                               m_bitmask);
  }
  // Remove components of an entity
  // template <typename... TargetComponets, typename... Components>
  // void rmComponents()
  // {
  //   m_scene->rmComponent<TargetComponets..., ObjectComponents...>(m_entity);
  // }

  explicit operator bool() const { return m_entity != -1; }

protected:
  template <typename arg> void initializeScript(arg &&nativeScriptComponent)
  {
    initializeScripts(m_scene, std::forward<arg>(nativeScriptComponent),
                      m_entity);
  }
  Entity m_entity = -1;
  Scene *m_scene;
  Bitmask m_bitmask;
  friend class Scene;
};

} // namespace pain
