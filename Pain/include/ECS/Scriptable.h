#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "ECS/Registry/Entity.h"
#include "ECS/Scene.h"

#include "GUI/ImGuiComponent.h"
namespace pain
{

// this function will replace GameObject, please remove GameObject later
struct NormalEntity {
public:
  template <typename... Components>
  std::tuple<Components &...> createComponents(Scene &scene,
                                               Components &&...args)
  {
    static_assert(sizeof...(Components) > 0,
                  "No components in the scripted object. Did you create "
                  "an object constructor but forgot to add its components?");
    return scene.createComponents<Components...>(
        m_entity, std::forward<Components>(args)...);
  }
  NormalEntity(Scene &scene) { m_entity = scene.createEntity(); }
  inline reg::Entity getEntity() const { return m_entity; }
  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  template <typename T> T &getComponent(Scene &scene)
  {
    return scene.getComponent<T>(m_entity);
  }
  template <typename T> const T &getComponent(const Scene &scene) const
  {
    return scene.getComponent<T>(m_entity);
  }
  template <typename... TargetComponents>
  std::tuple<TargetComponents &...> getComponents(Scene &scene)
  {
    return scene.getComponents<TargetComponents...>(m_entity);
  }
  template <typename... TargetComponents>
  std::tuple<TargetComponents &...> getComponents(const Scene &scene) const
  {
    return std::as_const(scene).getComponents<TargetComponents...>(m_entity);
  }

  // ------------------------------------------------------------ //
  // EMPLACE SCRIPT VARIANTS
  // ------------------------------------------------------------ //

  // This serves to quickly emplace a native script object inside the
  // NativeScriptComponent, it can also have arguments inside. Useful for
  // compile check
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  void emplaceScript(Scene &scene, Args... args)
  {
    NativeScriptComponent &nsc = getComponent<NativeScriptComponent>(scene);
    nsc.bindAndInitiate<T>(m_entity, scene, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }
  // This serves to quickly emplace a native script object inside the
  // ImGuiScriptComponent, it can also have arguments inside. Useful for
  // compile check.
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, Scene &, Args...>
  void emplaceImGuiScript(Scene &scene, Args... args)
  {
    ImGuiComponent &nsc = getComponent<ImGuiComponent>(scene);
    nsc.bindAndInitiate<T>(m_entity, scene, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }

  // ------------------------------------------------------------ //
  // Move constructors and assginment
  // ------------------------------------------------------------ //
  NormalEntity(NormalEntity &&other) noexcept
      : m_entity(std::exchange(other.m_entity, reg::Entity{-1})) {};
  NormalEntity &operator=(NormalEntity &&other) noexcept
  {
    if (this != &other) {
      m_entity = std::exchange(other.m_entity, reg::Entity{-1});
    }
    return *this;
  }

protected:
  reg::Entity m_entity = reg::Entity{0};
};

// ExtendedEntity, meaning that it has additional functions to more
// easily get components. This can be useful for scripts where the extra space
// isn't necessary
class ExtendedEntity
{
public:
  ExtendedEntity(reg::Entity entity, Scene &scene)
      : m_scene(scene), m_entity(entity) {};
  NONCOPYABLE(ExtendedEntity)
  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  template <typename... TargetComponents>
  std::tuple<TargetComponents &...> getComponents()
  {
    return m_scene.get().getComponents<TargetComponents...>(m_entity);
  }
  // return the components of an entity, as a tuple
  template <typename... TargetComponents>
  const std::tuple<TargetComponents &...> getComponents() const
  {
    return m_scene.get().getComponents<TargetComponents...>(m_entity);
  }
  template <typename T> T &getComponent()
  {
    PROFILE_FUNCTION();
    return m_scene.get().getComponent<T>(m_entity);
  }
  template <typename T> const T &getComponent() const
  {
    PROFILE_FUNCTION();
    return static_cast<const Scene &>(m_scene.get()).getComponent<T>(m_entity);
  }
  // ---------------------------------------------------- //
  // "Has" all components
  // ---------------------------------------------------- //
  // Does archetype has any of the target components?
  template <typename... TargetComponents> bool hasAnyComponents() const
  {
    return m_scene.get().hasAnyComponents<TargetComponents...>(m_entity);
  }

  // Does archetype has all of the target components?
  template <typename... TargetComponents> bool hasAllComponents() const
  {
    return m_scene.get().containsAllComponents<TargetComponents...>();
  }
  explicit operator bool() const { return m_entity != -1; }
  inline reg::Entity getEntity() const { return m_entity; }
  inline Scene &getScene() const { return m_scene.get(); }

  // ------------------------------------------------------------ //
  // MOVE CONSTRUCTORS AND ASSGINMENT
  // ------------------------------------------------------------ //
  ExtendedEntity(ExtendedEntity &&other) noexcept
      : m_scene(other.m_scene),
        m_entity(std::exchange(other.m_entity, reg::Entity{-1})) {};
  ExtendedEntity &operator=(ExtendedEntity &&other) noexcept
  {
    if (this != &other) {
      m_scene = other.m_scene;
      m_entity = std::exchange(other.m_entity, reg::Entity{-1});
    }
    return *this;
  }

protected:
  std::reference_wrapper<Scene> m_scene;
  reg::Entity m_entity;
  friend class Scene;
};

} // namespace pain
