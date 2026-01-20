/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file Scriptable.h
 * @brief Script-facing wrapper for entity and scene access.
 *
 * Provides a lightweight abstraction that allows scripts to safely interact
 * with entities and scenes without exposing low-level ECS details.
 *
 * GameObject acts as a typed gateway over a Scene and an Entity, enforcing
 * compile-time constraints on accessible components depending on the scene
 * type (world or UI).
 */
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

/**
 * @brief Compile-time traits describing which components are valid for a scene.
 *
 * SceneTraits maps a concrete scene type to its corresponding component
 * registry. It allows GameObject to restrict component access at compile time
 * depending on whether the script is running in a world scene or a UI scene.
 *
 * This prevents accidental access to components that are not registered in the
 * underlying scene configuration.
 *
 * @tparam SceneT Scene type.
 */
template <typename SceneT> struct SceneTraits;

/** @brief SceneTraits specialization for the world Scene. */
template <> struct SceneTraits<Scene> {
  /** @brief Checks whether a component type is registered in WorldComponents.
   */
  template <typename T> static constexpr bool isRegistered()
  {
    return WorldComponents::isRegistered<T>();
  }

  /** @brief Checks whether all component types are registered in
   * WorldComponents. */
  template <typename... Ts> static constexpr bool allRegistered()
  {
    return WorldComponents::allRegistered<Ts...>();
  }
};

/** @brief SceneTraits specialization for the UI scene. */
template <> struct SceneTraits<UIScene> {
  /** @brief Checks whether a component type is registered in UIComponents. */
  template <typename T> static constexpr bool isRegistered()
  {
    return UIComponents::isRegistered<T>();
  }

  /** @brief Checks whether all component types are registered in UIComponents.
   */
  template <typename... Ts> static constexpr bool allRegistered()
  {
    return UIComponents::allRegistered<Ts...>();
  }
};

// ------------------------------------------------------------
// GameObject
// ------------------------------------------------------------

/**
 * @brief Script-facing wrapper around an entity and its owning scene.
 *
 * GameObject provides controlled access to ECS functionality for gameplay and
 * UI scripts. It stores:
 *  - A reference to a scene.
 *  - A target entity within that scene.
 *
 * All component access is validated at compile time using SceneTraits to ensure
 * that only components registered in the scene configuration can be accessed.
 *
 * This class does not own the entity or the scene. It behaves as a lightweight
 * handle used by scripting systems.
 *
 * @tparam SceneT Scene type (Scene or UIScene).
 */
template <typename SceneT> class GameObject
{
public:
  using SceneType = SceneTraits<SceneT>;

  /**
   * @brief Constructs a GameObject bound to an entity and a scene.
   *
   * @param entity Target entity.
   * @param scene Owning scene.
   */
  GameObject(reg::Entity entity, SceneT &scene);

  /** @brief Factory helper for creating a GameObject wrapper. */
  static GameObject create(reg::Entity entity, SceneT &scene);

  // ------------------------------------------------------------
  // Self entity access
  // ------------------------------------------------------------

  /** @brief Retrieves multiple components from the wrapped entity. */
  template <typename... Ts> std::tuple<Ts &...> getComponents()
  {
    return getComponents<Ts...>(m_entity);
  }

  /** @brief Retrieves multiple components from the wrapped entity (const). */
  template <typename... Ts> std::tuple<const Ts &...> getComponents() const
  {
    return getComponents<Ts...>(m_entity);
  }

  /** @brief Retrieves a single component from the wrapped entity. */
  template <typename T> T &getComponent() { return getComponent<T>(m_entity); }

  /** @brief Retrieves a single component from the wrapped entity (const). */
  template <typename T> const T &getComponent() const
  {
    return getComponent<T>(m_entity);
  }

  // ------------------------------------------------------------
  // Other entity access
  // ------------------------------------------------------------

  /**
   * @brief Retrieves multiple components from another entity.
   *
   * Access is only allowed if all requested components are registered in the
   * scene configuration.
   */
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  std::tuple<Ts &...> getComponents(reg::Entity e)
  {
    return m_scene.get().template getComponents<Ts...>(e);
  }

  /** @brief Retrieves multiple components from another entity (const). */
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  std::tuple<const Ts &...> getComponents(reg::Entity e) const
  {
    return m_scene.get().template getComponents<Ts...>(e);
  }

  /** @brief Retrieves a single component from another entity. */
  template <typename T>
    requires(SceneType::template isRegistered<T>())
  T &getComponent(reg::Entity e)
  {
    return m_scene.get().template getComponent<T>(e);
  }

  /** @brief Retrieves a single component from another entity (const). */
  template <typename T>
    requires(SceneType::template isRegistered<T>())
  const T &getComponent(reg::Entity e) const
  {
    return m_scene.get().template getComponent<T>(e);
  }

  // ------------------------------------------------------------
  // Script Helpers
  // ------------------------------------------------------------

  /**
   * @brief Retrieves a native script instance from an entity.
   *
   * Only available if NativeScriptComponent is registered for the scene type.
   */
  template <typename S>
    requires(SceneType::template isRegistered<NativeScriptComponent>())
  S &getNativeScript(reg::Entity entity)
  {
    NativeScriptComponent &nsc = getComponent<NativeScriptComponent>(entity);
    return static_cast<S &>(*nsc.instance);
  }

  /**
   * @brief Retrieves an ImGui script instance from an entity.
   *
   * Only available if ImGuiComponent is registered for the scene type.
   */
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

  /** @brief Removes an entity from the scene. */
  void removeEntity(reg::Entity entity);

  // ------------------------------------------------------------
  // Queries
  // ------------------------------------------------------------

  /** @brief Checks whether the wrapped entity owns any of the given components.
   */
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  bool hasAnyComponents() const
  {
    return m_scene.get().template hasAnyComponents<Ts...>(m_entity);
  }

  /** @brief Checks whether the wrapped entity owns all of the given components.
   */
  template <typename... Ts>
    requires(SceneType::template allRegistered<Ts...>())
  bool hasAllComponents() const
  {
    return m_scene.get().template hasAllComponents<Ts...>(m_entity);
  }

  // ------------------------------------------------------------
  // Misc
  // ------------------------------------------------------------

  /** @brief Returns the wrapped entity identifier. */
  reg::Entity getEntity() const { return m_entity; }

  /** @brief Returns the scene event dispatcher. */
  reg::EventDispatcher &getEventDispatcher();
  const reg::EventDispatcher &getEventDispatcher() const;

  /** @brief Returns the owning scene. */
  SceneT &getScene();
  const SceneT &getScene() const;

  /** @brief Returns the scene thread pool. */
  ThreadPool &getThreadPool();
  const ThreadPool &getThreadPool() const;

private:
  aRefWrap<SceneT> m_scene;
  reg::Entity m_entity;
};

// Explicit instantiation declarations
extern template class GameObject<Scene>;
extern template class GameObject<UIScene>;

/** @brief Alias for a world script object. */
using WorldObject = GameObject<Scene>;

/** @brief Alias for a UI script object. */
using UIObject = GameObject<UIScene>;

} // namespace pain
