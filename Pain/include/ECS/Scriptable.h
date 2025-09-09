#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"

#include "ECS/Components/NativeScript.h"
#include "GUI/ImGuiComponent.h"
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
    m_entity = scene.createEntity();
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
  static constexpr Bitmask getBitMask()
  {
    return ComponentManager::multiComponentBitmask<Components...>();
  }
  template <typename T, typename... Args>
  void withScript(Scene &scene, Args... args)
  {
    NativeScriptComponent &nsc = getComponent<NativeScriptComponent>(scene);
    nsc.bindAndInitiate<T>(scene, m_entity, getBitMask(),
                           std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }
  template <typename T, typename... Args>
  void withImGuiScript(Scene &scene, Args... args)
  {
    ImGuiComponent &nsc = getComponent<ImGuiComponent>(scene);
    nsc.bindAndInitiate<T>(scene, m_entity, getBitMask(),
                           std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
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
  ExtendedEntity(Scene &scene, Entity entity, Bitmask bitmask)
      : m_scene(scene), m_entity(entity), m_bitmask(bitmask) {};
  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  template <typename... TargetComponents>
  std::tuple<TargetComponents &...> getComponents()
  {
    return m_scene.get().getComponents<TargetComponents...>(m_entity,
                                                            m_bitmask);
  }
  // return the components of an entity, as a tuple
  template <typename... TargetComponents>
  const std::tuple<TargetComponents &...> getComponents() const
  {
    return m_scene.get().getComponents<TargetComponents...>(m_entity,
                                                            m_bitmask);
  }
  template <typename T> T &getComponent()
  {
    PROFILE_FUNCTION();
    return m_scene.get().getComponent<T>(m_entity, m_bitmask);
  }
  template <typename T> const T &getComponent() const
  {
    PROFILE_FUNCTION();
    return static_cast<const Scene &>(m_scene.get())
        .getComponent<T>(m_entity, m_bitmask);
  }
  // ---------------------------------------------------- //
  // "Has" all components
  // ---------------------------------------------------- //
  // Does archetype has any of the target components?
  template <typename... TargetComponents> bool hasAnyComponents() const
  {
    return m_scene.get().hasAnyComponents<TargetComponents...>(m_bitmask);
  }

  // Does archetype has all of the target components?
  template <typename... TargetComponents> bool hasAllComponents() const
  {
    return m_scene.get().containsAllComponents<TargetComponents...>(m_bitmask);
  }
  explicit operator bool() const { return m_entity != -1; }

protected:
  std::reference_wrapper<Scene> m_scene;
  Entity m_entity;
  Bitmask m_bitmask;
  friend class Scene;
};

} // namespace pain
