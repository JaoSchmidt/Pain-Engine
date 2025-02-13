#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/IteratorArchetype.h"
#include "pch.h"

#include "Core.h"

#include "ECS/Entity.h"
#include <tuple>
#include <vector>

namespace pain
{

template <typename T>
concept Component = requires {
  { T::ComponentID } -> std::convertible_to<int>;
};

class ArcheRegistry
{
  friend class Scene;
  ArcheRegistry() = default;
  // ---------------------------------------------------- //
  // add and remove
  // ---------------------------------------------------- //

  // Add an entity with N different components, passing N arguments
  template <typename... Components, typename... Args>
  Tuple<Components &...> addComponents(Entity entity, Args &&...args)
  {
    static_assert(sizeof...(Components) == sizeof...(Args),
                  "Number of components and arguments must match.");
    Archetype<Components...> &archetype = getOrCreateArchetype<Components...>();
    (archetype.template pushComponent<Components>(std::forward<Args>(args)),
     ...);
    return archetype.template extractColumn<Components...>(entity);
  }
  // TODO: Add component should move entity to different archetype
  template <typename T, typename... Args>
  T &addComponent(Entity entity, Args &&...args)
  {
    Archetype &archetype = getOrCreateArchetype<T>();
    archetype.pushComponent<T>(std::forward<Args>(args)...);
    return archetype.extractComponent<T>(entity);
  }

  // ---------------------------------------------------- //
  // get
  // ---------------------------------------------------- //

  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... Components>
  Tuple<Components &...> &getComponents(Entity entity)
  {
    Archetype &archetype = getOrCreateArchetype<Components...>();
    return archetype.extractColumn<Components...>(entity);
  }
  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... Components>
  const Tuple<Components &...> &getComponents(Entity entity) const
  {
    Archetype &archetype = getOrCreateArchetype<Components...>();
    return archetype.extractColumn<Components...>(entity);
  }
  // A component of an entity
  template <typename T> T &getComponent(Entity entity)
  {
    Archetype &archetype = getOrCreateArchetype<T>();
    return archetype.extractComponent<T>(entity);
  }
  // A component of an entity
  template <typename T> const T &getComponent(Entity entity) const
  {
    Archetype &archetype = getOrCreateArchetype<T>();
    return archetype.extractComponent<T>(entity);
  }

  template <typename... Components> bool has(Entity entity) const
  {

    int bitMask = getBitMask<Components...>();
    auto it = archetypes.find(bitMask);
    if (it == archetypes.end())
      return false;
    return it->second.has(entity);
  }

  /* Remove a single column of an archetype, which represents the components
  of an entity. In this function, the components have to be provided to remove
  the entity, allowing it to remove faster. For a more generic and slower
  function, call without providing the components If entity is found, then 0 is
  returned, otherwise 0 is returned*/
  template <typename... Components> void remove(Entity entity)
  {
    Archetype &archetype = getOrCreateArchetype<Components...>();
    archetype.remove(entity);
  }

  // Get an iterator for an entire collection of vectors
  template <typename C>
  reg::Iterator<C>
  begin(const std::vector<std::vector<C>> &vectors,
        const std::vector<const std::vector<pain::Entity> *> &entities)
  {
    return reg::Iterator<C>(vectors, 0, 0, entities);
  }
  template <typename C>
  const reg::Iterator<C>
  end(const std::vector<std::vector<C>> &vectors,
      const std::vector<const std::vector<pain::Entity> *> &entities) const
  {
    return reg::Iterator<C>(vectors, vectors.size(), 0, entities);
  }

  // return tuple of iterators
  template <typename... Components> Tuple<reg::Iterator<Components>...> begin()
  {
    int bitMask = getBitMask<Components...>();
    // filter archetypes
    std::vector<Archetype *> archetypes = filterArchetype(bitMask);
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(
        extractComponentFromArchetypes<Components>(archetypes)...);
  }
  // return tuple of iterators
  template <typename... Components>
  const Tuple<reg::Iterator<Components>...> end() const
  {
    int bitMask = getBitMask<Components...>();
    // filter archetypes
    std::vector<const Archetype *> archetypes = filterArchetype(bitMask);
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(
        extractComponentFromArchetypes<Components>(archetypes)...);
  }

  // Extract all vectors of that particular component from the archetypes
  template <typename C>
  reg::Iterator<C>
  createIterateFromComponentBegin(std::vector<Archetype *> archetypes) const
  {
    std::vector<std::vector<void *> *> vectors;
    vectors.reserve(archetypes.size());
    for (const Archetype *archetype : archetypes) {
      std::vector<void *> componentVector =
          archetype->componentVector(getComponentID<C>());
      vectors.push_back(static_cast<std::vector<C> *>(componentVector));
    }

    std::vector<const std::vector<Entity> *> entities;
    entities.reserve(archetypes.size());
    for (const Archetype *archetype : archetypes) {
      entities.push_back(&(archetype->m_entities));
    }
    return reg::Iterator(vectors, 0, 0, entities);
  }

  template <typename C>
  const reg::Iterator<C>
  createIterateFromComponentEnd(std::vector<Archetype *> archetypes) const
  {
    std::vector<std::vector<void *> *> vectors;
    vectors.reserve(archetypes.size());
    for (const Archetype *archetype : archetypes) {
      std::vector<void *> componentVector =
          archetype->componentVector(getComponentID<C>());
      vectors.push_back(static_cast<std::vector<C> *>(componentVector));
    }

    std::vector<const std::vector<Entity> *> entities;
    entities.reserve(archetypes.size());
    for (const Archetype *archetype : archetypes) {
      entities.push_back(&(archetype->m_entities));
    }
    return reg::Iterator(vectors, 0, 0, entities);
  }

  // ---------------------------------------------------- //
  // Maps
  // ---------------------------------------------------- //

  // get bit mask corresponding to that set of arrays
  template <typename... Components> constexpr int getBitMask()
  {
    // https://en.cppreference.com/w/cpp/language/fold
    return (getComponentID<Components>() | ...);
  }
  template <typename C> constexpr int getComponentID()
  {
    return C::componentID;
  }

  template <typename... Components> Archetype &getOrCreateArchetype()
  {
    int bitMask = getBitMask<Components...>();
    auto it = archetypes.find(bitMask);
    if (it == archetypes.end()) {
      auto pair = archetypes.emplace(bitMask, Archetype(bitMask));
      P_ASSERT(pair.second,
               "It wasn't possible to create archetype with bitmask {}",
               bitMask);
      return pair.first->second;
    }
    return it->second;
  }

  // Create archetype and return its number
  Archetype &createArchetype(int bitMask)
  {
    auto it = archetypes.find(bitMask);
    if (it == archetypes.end()) {
      auto pair = archetypes.emplace(bitMask, Archetype(bitMask));
      P_ASSERT(pair.second,
               "It wasn't possible to create archetype with bitmask {}",
               bitMask);
      return pair.first->second;
    }
    return it->second;
  }

  Archetype &getOrCreateArchetype(int bitMask)
  {
    auto it = archetypes.find(bitMask);
    if (it != archetypes.end()) {
      return it->second;
    }
    return createArchetype(bitMask);
  }

  // Filter archetypes to find those that have at least the components in the
  // bitmask
  const std::vector<const Archetype *> filterArchetype(int bitNumber) const
  {
    std::vector<const Archetype *> matchingArchetypes;
    for (const auto &[mask, archetype] : archetypes) {
      if ((mask & bitNumber) == bitNumber) {
        matchingArchetypes.push_back(&archetype);
      }
    }
    return matchingArchetypes;
  }
  std::vector<Archetype *> filterArchetype(int bitNumber)
  {
    std::vector<Archetype *> matchingArchetypes;
    for (auto &[mask, archetype] : archetypes) {
      if ((mask & bitNumber) == bitNumber) {
        matchingArchetypes.push_back(&archetype);
      }
    }
    return matchingArchetypes;
  }

  std::unordered_map<int, Archetype> archetypes;
};
} // namespace pain
