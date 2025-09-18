#pragma once
#include "ECS/Registry/ArcheRegistry.h"

namespace pain
{
struct System {
public:
  System(ArcheRegistry &archetype) : m_registry(archetype) {};

protected:
  ArcheRegistry &m_registry;

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename T>
    requires IsSingleType<T>
  inline reg::Iterator<T> end()
  {
    return m_registry.end<T>();
  }
  template <typename T>
    requires IsSingleType<T>
  inline reg::Iterator<T> begin()
  {
    return m_registry.begin<T>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  inline std::tuple<reg::Iterator<Components>...> begin()
  {
    return m_registry.begin<Components...>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  inline std::tuple<reg::Iterator<Components>...> end()
  {
    return m_registry.end<Components...>();
  }
};
} // namespace pain
