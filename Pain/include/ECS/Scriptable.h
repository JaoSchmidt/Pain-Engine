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

class SceneHandle
{
  enum class Kind { Scene, UIScene };

public:
  SceneHandle(Scene &s) : kind(Kind::Scene) { data.scene = &s; }
  SceneHandle(UIScene &s) : kind(Kind::UIScene) { data.uiscene = &s; }

  // ---------------- Components ---------------- //

  template <typename T> T &getComponent(reg::Entity e)
  {
    switch (kind) {
    case Kind::Scene:
      if constexpr (ComponentManager::isRegistered<T>())
        return data.scene->getComponent<T>(e);
      std::abort();
    case Kind::UIScene:
      if constexpr (UIManager::isRegistered<T>())
        return data.uiscene->getComponent<T>(e);
      std::abort();
    }
  }

  template <typename T> const T &getComponent(reg::Entity e) const
  {
    switch (kind) {
    case Kind::Scene:
      if constexpr (ComponentManager::isRegistered<T>())
        return std::as_const(*data.scene).getComponent<T>(e);
      std::abort();
    case Kind::UIScene:
      if constexpr (UIManager::isRegistered<T>())
        return std::as_const(*data.uiscene).getComponent<T>(e);
      std::abort();
    }
  }

  template <typename... Ts> std::tuple<Ts &...> getComponents(reg::Entity e)
  {
    switch (kind) {
    case Kind::Scene:
      if constexpr (ComponentManager::allRegistered<Ts...>())
        return data.scene->getComponents<Ts...>(e);
      std::abort();
    case Kind::UIScene:
      if constexpr (UIManager::allRegistered<Ts...>())
        return data.uiscene->getComponents<Ts...>(e);
      std::abort();
    }
  }

  template <typename... Ts>
  std::tuple<const Ts &...> getComponents(reg::Entity e) const
  {
    switch (kind) {
    case Kind::Scene:
      if constexpr (ComponentManager::allRegistered<Ts...>())
        return std::as_const(*data.scene).getComponents<Ts...>(e);
      std::abort();
    case Kind::UIScene:
      if constexpr (UIManager::allRegistered<Ts...>())
        return std::as_const(*data.uiscene).getComponents<Ts...>(e);
      std::abort();
    }
  }

  // ---------------- Queries ---------------- //

  template <typename... Ts> bool hasAnyComponents(reg::Entity e) const
  {
    switch (kind) {
    case Kind::Scene:
      if constexpr (ComponentManager::allRegistered<Ts...>())
        return data.scene->hasAnyComponents<Ts...>(e);
    case Kind::UIScene:
      if constexpr (UIManager::allRegistered<Ts...>())
        return data.uiscene->hasAnyComponents<Ts...>(e);
    }
    std::abort();
  }

  template <typename... Ts> bool hasAllComponents(reg::Entity e) const
  {
    switch (kind) {
    case Kind::Scene:
      if constexpr (ComponentManager::allRegistered<Ts...>())
        return data.scene->containsAllComponents<Ts...>(e);
    case Kind::UIScene:
      if constexpr (UIManager::allRegistered<Ts...>())
        return data.uiscene->containsAllComponents<Ts...>(e);
    }
    std::abort();
  }

  // ---------------- Misc ---------------- //

  reg::EventDispatcher &getEventDispatcher()
  {
    switch (kind) { // NOTE: it doesn't the branch in this case, but the right
                    // scene still needs to be selected here;
    case Kind::Scene:
      return data.scene->getEventDispatcher();
    case Kind::UIScene:
      return data.uiscene->getEventDispatcher();
    }
    std::abort();
  }

  const reg::EventDispatcher &getEventDispatcher() const
  {
    switch (kind) { // NOTE: it doesn't the branch in this case, but the right
                    // scene still needs to be selected here;
    case Kind::Scene:
      return std::as_const(*data.scene).getEventDispatcher();
    case Kind::UIScene:
      return std::as_const(*data.scene).getEventDispatcher();
    }
    std::abort();
  }

  const Kind kind;
  union {
    Scene *scene;
    UIScene *uiscene;
  } data;
};

// ExtendedEntity, meaning that it has additional functions to more
// easily get components. This can be useful for scripts where the extra space
// isn't necessary
class ExtendedEntity
{
public:
  ExtendedEntity(reg::Entity entity, Scene &scene)
      : m_scene(scene), m_entity(entity) {};

  ExtendedEntity(reg::Entity entity, UIScene &scene)
      : m_scene(scene), m_entity(entity) {};

  // ---------------------------------------------------- //
  // Self entity convenience
  // ---------------------------------------------------- //

  template <typename... Ts> std::tuple<Ts &...> getComponents()
  {
    return getComponents<Ts...>(m_entity);
  }

  template <typename... Ts> std::tuple<const Ts &...> getComponents() const
  {
    return getComponents<Ts...>(m_entity);
  }

  template <typename T> T &getComponent() { return getComponent<T>(m_entity); }

  template <typename T> const T &getComponent() const
  {
    return getComponent<T>(m_entity);
  }

  // ---------------------------------------------------- //
  // Other entity access
  // ---------------------------------------------------- //

  template <typename... Ts> std::tuple<Ts &...> getComponents(reg::Entity e)
  {
    return m_scene.getComponents<Ts...>(e);
  }

  template <typename... Ts>
  std::tuple<const Ts &...> getComponents(reg::Entity e) const
  {
    return m_scene.getComponents<Ts...>(e);
  }

  template <typename T> T &getComponent(reg::Entity e)
  {
    return m_scene.getComponent<T>(e);
  }

  template <typename T> const T &getComponent(reg::Entity e) const
  {
    return m_scene.getComponent<T>(e);
  }

  // ---------------------------------------------------- //
  // Queries
  // ---------------------------------------------------- //

  template <typename... Ts> bool hasAnyComponents() const
  {
    return m_scene.hasAnyComponents<Ts...>(m_entity);
  }

  template <typename... Ts> bool hasAllComponents() const
  {
    return m_scene.hasAllComponents<Ts...>(m_entity);
  }

  // ---------------------------------------------------- //
  // Misc
  // ---------------------------------------------------- //

  explicit operator bool() const { return m_entity != reg::Entity{-1}; }

  reg::Entity getEntity() const { return m_entity; }

  reg::EventDispatcher &getEventDispatcher()
  {
    return m_scene.getEventDispatcher();
  }

  const reg::EventDispatcher &getEventDispatcher() const
  {
    return m_scene.getEventDispatcher();
  }

  // ---------------------------------------------------- //
  // Move support
  // ---------------------------------------------------- //

  ExtendedEntity(ExtendedEntity &&other) noexcept
      : m_scene(other.m_scene),
        m_entity(std::exchange(other.m_entity, reg::Entity{-1})) {};
  ExtendedEntity(const ExtendedEntity &other) noexcept
      : m_scene(other.m_scene), m_entity(other.m_entity) {};

  ExtendedEntity &operator=(ExtendedEntity &&other) = delete;
  ExtendedEntity &operator=(const ExtendedEntity &other) = delete;

protected:
  SceneHandle m_scene;
  reg::Entity m_entity;
};
} // namespace pain
