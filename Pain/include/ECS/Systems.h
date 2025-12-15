// Systems.h
#pragma once

#include "ECS/Registry/ArcheRegistry.h"
#include <iostream>

namespace pain
{
template <typename CM> struct System {
public:
  System(reg::ArcheRegistry<CM> &archetype) : m_registry(archetype) {};

  // Move constructor
  System(System &&other) noexcept : m_registry(other.m_registry)
  {
    std::cout << "System move-constructed\n";
  }

  // Move assignment
  System &operator=(System &&other) noexcept
  {
    std::cout << "System move-assigned\n";
    if (this != &other) {
      // you cannot rebind a reference, so just keep the old one
      // m_registry still refers to the same ArcheRegistry
      // but we can log this for clarity
    }
    return *this;
  }

  // Delete copy operations to be explicit
  System(const System &) = delete;
  System &operator=(const System &) = delete;

protected:
  reg::ArcheRegistry<CM> &m_registry;

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
  inline std::vector<reg::ChunkView<const Components...>>
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
};
} // namespace pain
