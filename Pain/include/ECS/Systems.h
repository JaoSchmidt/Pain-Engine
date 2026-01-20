/**
 * @file Systems.h
 * @brief Base interfaces and utilities for ECS systems.
 *
 * Defines the core system abstraction used by the engine. Systems operate on
 * ECS archetypes, react to events, update game state, and optionally render.
 *
 * Concrete systems (e.g. rendering, ImGui, gameplay logic) derive from
 * System<Components> and implement one or more lifecycle interfaces:
 *  - IOnUpdate
 *  - IOnEvent
 *  - IOnRender
 *
 * Compile-time concepts enforce correct inheritance and component usage.
 */

#pragma once

#include "Core.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/EventDispatcher.h"
#include "ECS/Registry/ArcheRegistry.h"
#include <iostream>

namespace pain
{
class DeltaTime;
struct Renderer2d;

/**
 * @brief Compile-time typelist used for tagging systems.
 *
 * Used by systems to declare the component types they depend on. This allows
 * scenes to validate at compile time that all required components are
 * registered before inserting a system.
 *
 * Example:
 * @code
 * using Tags = TypeList<Transform, Sprite>;
 * @endcode
 */
template <typename... Ts> struct TypeList {
};

/** @brief Interface for systems that update every frame. */
struct IOnUpdate {
  virtual ~IOnUpdate() = default;

  /**
   * @brief Called once per frame during the update phase.
   *
   * @param dt Frame delta time.
   */
  virtual void onUpdate(DeltaTime dt) = 0;
};

/** @brief Interface for systems that receive input or engine events. */
struct IOnEvent {
  virtual ~IOnEvent() = default;

  /**
   * @brief Called when an SDL event is dispatched.
   *
   * @param event Incoming SDL event.
   */
  virtual void onEvent(const SDL_Event &event) = 0;
};

/// @brief Interface for systems that participate in rendering.
struct IOnRender {
  virtual ~IOnRender() = default;

  /**
   * @brief Called during the render phase.
   *
   * @param renderer Active 2D renderer.
   * @param debug Whether debug rendering is enabled.
   * @param dt Frame delta time.
   */
  virtual void onRender(Renderer2d &renderer, bool debug, DeltaTime dt) = 0;
};

/**
 * @brief Base class for all ECS systems.
 *
 * A System provides access to:
 *  - The archetype registry.
 *  - The event dispatcher.
 *
 * It exposes helper utilities for querying archetypes, accessing components,
 * and managing entities.
 *
 * Concrete systems typically inherit from:
 * @code
 * class MySystem : public System<WorldComponents>, public IOnUpdate { ... };
 * @endcode
 *
 * @tparam CM Component manager type associated with the scene.
 */
template <typename CM> struct System {
public:
  /**
   * @brief Constructs a system bound to a registry and event dispatcher.
   *
   * @param archetype ECS archetype registry.
   * @param eventDispatcher Event dispatcher used for publishing and receiving
   * events.
   */
  System(reg::ArcheRegistry<CM> &archetype,
         reg::EventDispatcher &eventDispatcher)
      : m_registry(archetype), m_eventDispatcher(eventDispatcher) {};

  System(System &&other) = default;
  System &operator=(System &&other) = delete;
  NONCOPYABLE(System);
  System() = delete;
  virtual ~System() = default;

  /** @brief Reference to the ECS archetype registry. */
  reg::ArcheRegistry<CM> &m_registry;

  /** @brief Reference to the event dispatcher. */
  reg::EventDispatcher &m_eventDispatcher;

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  /**
   * @brief Queries archetypes containing a set of components.
   *
   * This overload exists when component bitmasks cannot be deduced at compile
   * time. The system can still run, but it cannot be validated when inserted
   * into a scene.
   *
   * @deprecated Prefer the constrained overload when possible.
   */
  template <typename... Components, typename... ExcludeComponents>
  [[deprecated("Bitmask cannot be deduced for one the components in this "
               "function. This is fine to call, but you won't be able to add "
               "this system to any scene")]]
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template query<Components...>(
        exclude<ExcludeComponents...>);
  }

  /**
   * @brief Queries archetypes containing a set of registered components.
   *
   * Only enabled when all requested component types are registered in the
   * component manager.
   *
   * @return A list of chunk views matching the query.
   */
  template <typename... Components, typename... ExcludeComponents>
    requires(CM::template allRegistered<Components...>())
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template query<Components...>(
        exclude<ExcludeComponents...>);
  }

  /** @brief Const version of query(). */
  template <typename... Components, typename... ExcludeComponents>
    requires(CM::template allRegistered<Components...>())
  inline std::vector<reg::ChunkViewConst<const Components...>>
  queryConst(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template queryConst<Components...>(
        exclude<ExcludeComponents...>);
  }

  // ---------------------------------------------------- //
  // Sizes
  // ---------------------------------------------------- //

  /** @brief Returns the number of iterators produced by a query. */
  template <typename... Components, typename... ExcludeComponents>
    requires reg::IsMultipleTypes<Components...>
  size_t iteratorSize(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template iteratorSize<Components...>(
        exclude<ExcludeComponents...>);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //

  /** @brief Retrieves multiple components from an entity. */
  template <typename... Components>
  std::tuple<Components &...> getComponents(reg::Entity entity)
  {
    return m_registry.template getComponents<Components...>(entity);
  }

  /** @brief Retrieves multiple components from an entity (const). */
  template <typename... Components>
  const std::tuple<const Components &...>
  getComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template getComponents<Components...>(entity);
  }

  /** @brief Retrieves a single component from an entity. */
  template <typename T> T &getComponent(reg::Entity entity)
  {
    return m_registry.template getComponent<T>(entity);
  }

  /** @brief Retrieves a single component from an entity (const). */
  template <typename T> const T &getComponent(reg::Entity entity) const
  {
    return std::as_const(m_registry).template getComponent<T>(entity);
  }

  // ---------------------------------------------------- //
  // "Has" functions
  // ---------------------------------------------------- //

  /** @brief Checks whether an entity owns any of the specified components. */
  template <typename... TargetComponents>
  bool hasAnyComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template hasAny<TargetComponents...>(entity);
  }

  /** @brief Checks whether an entity owns all of the specified components. */
  template <typename... TargetComponents>
  constexpr bool containsAllComponents(reg::Entity entity) const
  {
    return m_registry.template containsAll<TargetComponents...>(entity);
  }

  /**
   * @brief Removes an entity and its components from the registry.
   *
   * @note The caller must specify the exact component set of the entity.
   */
  template <typename... Components> bool removeEntity(reg::Entity entity)
  {
    return m_registry.template remove<Components...>(entity);
  }
};

// ------------------------------------------------------------
// System Concepts & Validation
// ------------------------------------------------------------

/** @brief Forward declaration used for validating system tags. */
template <typename TL> struct TagsAllRegistered;

/** @brief Checks whether a type derives from the base world system. */
template <typename T>
concept IsSystemBase = std::is_base_of_v<System<WorldComponents>, T>;

/** @brief Checks whether a system implements at least one lifecycle interface.
 */
template <typename T>
concept HasAnySystemInterface =
    std::is_base_of_v<IOnUpdate, T> || std::is_base_of_v<IOnEvent, T> ||
    std::is_base_of_v<IOnRender, T>;

/** @brief Checks whether a system defines a Tags type. */
template <typename T>
concept HasTags = requires { typename T::Tags; };

/** @brief Checks whether all system tags are registered components.*/
template <typename T>
concept TagsAreRegistered =
    HasTags<T> && TagsAllRegistered<typename T::Tags>::value;

/**
 * @brief Valid system concept.
 *
 * A valid system must:
 *  - Derive from System<WorldComponents>.
 *  - Implement at least one system interface (update, render, or event).
 */
template <typename T>
concept ValidSystem = IsSystemBase<T> && HasAnySystemInterface<T>;

} // namespace pain
