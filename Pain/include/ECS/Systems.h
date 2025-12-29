// Systems.h
#pragma once

#include "Core.h"
#include "ECS/EventDispatcher.h"
#include "ECS/Registry/ArcheRegistry.h"
#include <iostream>

namespace pain
{
class DeltaTime;
struct Renderer2d;

struct IOnUpdate {
  virtual ~IOnUpdate() = default;
  virtual void onUpdate(DeltaTime) = 0;
};

struct IOnEvent {
  virtual ~IOnEvent() = default;
  virtual void onEvent(const SDL_Event &) = 0;
};

struct IOnRender {
  virtual ~IOnRender() = default;
  virtual void onRender(Renderer2d &, bool, DeltaTime) = 0;
};

template <typename CM> struct System {
public:
  System(reg::ArcheRegistry<CM> &archetype,
         reg::EventDispatcher &eventDispatcher)
      : m_registry(archetype), m_eventDispatcher(eventDispatcher) {};

  System(System &&other) = default;
  System &operator=(System &&other) = delete;
  NONCOPYABLE(System);
  System() = delete;
  virtual ~System() = default;

protected:
  reg::ArcheRegistry<CM> &m_registry;
  reg::EventDispatcher &m_eventDispatcher;

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template query<Components...>(
        exclude<ExcludeComponents...>);
  }
  template <typename... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkViewConst<const Components...>>
  queryConst(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template queryConst<Components...>(
        exclude<ExcludeComponents...>);
  }

  // ---------------------------------------------------- //
  // Sizes
  // ---------------------------------------------------- //

  // return the size of the iterators
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
  template <typename... Components>
  std::tuple<Components &...> getComponents(reg::Entity entity)
  {
    return m_registry.template getComponents<Components...>(entity);
  }
  // get multiple components together as a tuple
  template <typename... Components>
  const std::tuple<const Components &...>
  getComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template getComponents<Components...>(entity);
  }
  // get a single component
  template <typename T> T &getComponent(reg::Entity entity)
  {
    return m_registry.template getComponent<T>(entity);
  }
  // get a single component
  template <typename T> const T &getComponent(reg::Entity entity) const
  {
    return std::as_const(m_registry).template getComponent<T>(entity);
  }

  // ---------------------------------------------------- //
  // "Has" functions
  // ---------------------------------------------------- //
  template <typename... TargetComponents>
  bool hasAnyComponents(reg::Entity entity) const
  {
    return std::as_const(m_registry)
        .template hasAny<TargetComponents...>(entity);
  }
  template <typename... TargetComponents>
  constexpr bool containsAllComponents(reg::Entity entity) const
  {
    return m_registry.template containsAll<TargetComponents...>(entity);
  }
  // remove an entity, alongside its components from it's archetype,
  // NOTE: The caller needs to tell me the components of the entity
  template <typename... Components> bool removeEntity(reg::Entity entity)
  {
    return m_registry.template remove<Components...>(entity);
  }
};
} // namespace pain
