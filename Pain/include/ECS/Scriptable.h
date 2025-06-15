#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"
class Scene;

namespace pain
{

// this function will replace GameObject, please remove GameObject later
template <typename... Components> class NormalEntity
{
public:
  std::tuple<Components &...> createComponents(Scene &scene,
                                               Components &&...args)
  {
    static_assert(sizeof...(Components) > 0,
                  "No components in the scripted object. Did you create "
                  "an object constructor but forgot to add its components?");
    return scene.createComponents<Components...>(
        m_entity, std::forward<Components>(args)...);
  }
  template <typename T> T &getComponent(Scene &scene)
  {
    return scene.getComponent<T>(m_entity, getBitMask());
  }
  template <typename T> const T &getComponent(const Scene &scene) const
  {
    return scene.getComponent<T>(m_entity, getBitMask());
  }
  constexpr Bitmask getBitMask() const
  {
    return ComponentManager::multiComponentBitmask<Components...>();
  }
  NormalEntity(Scene &scene) { m_entity = scene.createEntity(); }
  inline Entity getEntity() const { return m_entity; }

protected:
  Entity m_entity = 0;
};

// ExtendedEntity, meaning that it has additional functions to more
// easily get components. This can be useful for scripts where the extra space
// isn't necessary
class ExtendedEntity
{
public:
  ExtendedEntity() = default;
  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  template <typename... TargetComponents>
  std::tuple<TargetComponents &...> getComponents()
  {
    return m_scene->getComponents<TargetComponents...>(m_entity, m_bitmask);
  }
  // return the components of an entity, as a tuple
  template <typename... TargetComponents>
  const std::tuple<TargetComponents &...> getComponents() const
  {
    return m_scene->getComponents<TargetComponents...>(m_entity, m_bitmask);
  }
  template <typename T> T &getComponent()
  {
    PROFILE_FUNCTION();
    return m_scene->getComponent<T>(m_entity, m_bitmask);
  }
  template <typename T> const T &getComponent() const
  {
    PROFILE_FUNCTION();
    return static_cast<const Scene *>(m_scene)->getComponent<T>(m_entity,
                                                                m_bitmask);
  }
  // ---------------------------------------------------- //
  // "Has" all components
  // ---------------------------------------------------- //
  // Does archetype has any of the target components?
  template <typename... TargetComponents> bool hasAnyComponents() const
  {
    return m_scene->hasAnyComponents<TargetComponents...>(m_bitmask);
  }

  // Does archetype has all of the target components?
  template <typename... TargetComponents> bool hasAllComponents() const
  {
    return m_scene->containsAllComponents<TargetComponents...>(m_bitmask);
  }
  explicit operator bool() const { return m_entity != -1; }

protected:
  Entity m_entity;
  Bitmask m_bitmask;
  Scene *m_scene = nullptr;
  friend class Scene;
};

template <typename... Components>
class LuaScriptableEntity : public ExtendedEntity
{
  LuaScriptableEntity(sol::state &solState);
};
} // namespace pain
