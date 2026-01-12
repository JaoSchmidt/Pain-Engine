// Scriptable.h
#pragma once

#include "Core.h"
#include "ECS/Registry/Entity.h"
#include "ECS/Scene.h"
#include "UIScene.h"
#include "WorldScene.h"
#include "aliases.h"

namespace pain
{

// ------------------------------------------------------------
// SceneTraits
// ------------------------------------------------------------
//
template <typename SceneT> struct SceneTraits;
template <> struct SceneTraits<Scene> {
  template <typename T> static constexpr bool isRegistered()
  {
    return WorldComponents::isRegistered<T>();
  }
  template <typename... Ts> static constexpr bool allRegistered()
  {
    return WorldComponents::allRegistered<Ts...>();
  }
};
template <> struct SceneTraits<UIScene> {
  template <typename T> static constexpr bool isRegistered()
  {
    return UIComponents::isRegistered<T>();
  }
  template <typename... Ts> static constexpr bool allRegistered()
  {
    return UIComponents::allRegistered<Ts...>();
  }
};

// ------------------------------------------------------------
// GameObject
// ------------------------------------------------------------

template <typename SceneT> class GameObject
{
public:
  using SceneType = SceneTraits<SceneT>;
  GameObject<SceneT>(reg::Entity entity, SceneT &scene);

  static GameObject<SceneT> create(reg::Entity entity, SceneT &scene);

  // ------------------------------------------------------------
  // Self entity access
  // ------------------------------------------------------------
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

  // ------------------------------------------------------------
  // Other entity access
  // ------------------------------------------------------------
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  std::tuple<Ts &...> getComponents(reg::Entity e)
  {
    return m_scene.get().template getComponents<Ts...>(e);
  }
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  std::tuple<const Ts &...> getComponents(reg::Entity e) const
  {
    return m_scene.get().template getComponents<Ts...>(e);
  }
  template <typename T>
    requires(SceneType::template isRegistered<T>())
  T &getComponent(reg::Entity e)
  {
    return m_scene.get().template getComponent<T>(e);
  }
  template <typename T>
    requires(SceneType::template isRegistered<T>())
  const T &getComponent(reg::Entity e) const
  {
    return m_scene.get().template getComponent<T>(e);
  }

  // ------------------------------------------------------------
  // Script Helpers
  // ------------------------------------------------------------

  template <typename S>
    requires(SceneType::template isRegistered<NativeScriptComponent>())
  S &getNativeScript(reg::Entity entity)
  {
    NativeScriptComponent &nsc = getComponent<NativeScriptComponent>(entity);
    return static_cast<S &>(*nsc.instance);
  }

  template <typename S>
    requires(SceneType::template isRegistered<ImGuiComponent>())
  S &getImGuiScript(reg::Entity entity)
  {
    ImGuiComponent &nsc = getComponent<ImGuiComponent>(entity);
    return static_cast<S &>(*nsc.instance);
  }

  // ------------------------------------------------------------
  // Removal
  // ------------------------------------------------------------

  void removeEntity(reg::Entity entity);

  // ------------------------------------------------------------
  // Queries
  // ------------------------------------------------------------
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  bool hasAnyComponents() const
  {
    return m_scene.get().template hasAnyComponents<Ts...>(m_entity);
  }
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  bool hasAllComponents() const
  {
    return m_scene.get().template hasAllComponents<Ts...>(m_entity);
  }

  // ------------------------------------------------------------
  // Misc
  // ------------------------------------------------------------
  reg::Entity getEntity() const { return m_entity; }
  reg::EventDispatcher &getEventDispatcher();
  const reg::EventDispatcher &getEventDispatcher() const;

  SceneT &getScene();
  const SceneT &getScene() const;

  ThreadPool &getThreadPool();
  const ThreadPool &getThreadPool() const;

private:
  aRefWrap<SceneT> m_scene;
  reg::Entity m_entity;
};
// Explicit instantiation declarations
extern template class GameObject<Scene>;
extern template class GameObject<UIScene>;

using WorldObject = GameObject<Scene>;
using UIObject = GameObject<UIScene>;

} // namespace pain
