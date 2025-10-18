#pragma once
#include "ECS/Registry/ArcheRegistry.h"
#include <iostream>

namespace pain
{
struct System {
public:
  System(ArcheRegistry &archetype) : m_registry(archetype) {};
  ~System() { std::cout << "System destructed\n"; }

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

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  template <typename... Components>
  std::tuple<Components &...> getComponents(Entity entity, Bitmask bitmask)
  {
    return m_registry.getComponents<Components...>(entity, bitmask);
  }
  // get multiple components together as a tuple
  template <typename... Components>
  const std::tuple<const Components &...> getComponents(Entity entity,
                                                        Bitmask bitmask) const
  {
    return std::as_const(m_registry)
        .getComponents<Components...>(entity, bitmask);
  }
  // get a single component
  template <typename T> T &getComponent(Entity entity, Bitmask bitmask)
  {
    return m_registry.getComponent<T>(entity, bitmask);
  }
  // get a single component
  template <typename T>
  const T &getComponent(Entity entity, Bitmask bitmask) const
  {
    return std::as_const(m_registry).getComponent<T>(entity, bitmask);
  }
};
} // namespace pain
