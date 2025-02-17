#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/IteratorArchetype.h"
#include "pch.h"

#include "Core.h"

#include "ECS/Entity.h"
#include <tuple>
#include <type_traits>
#include <vector>

namespace pain
{

// https://stackoverflow.com/questions/25958259/how-do-i-find-out-if-a-tuple-contains-a-type/25958302#25958302
template <typename T, typename Tup> struct has_type;

template <typename T> struct has_type<T, std::tuple<>> : std::false_type {
};

template <typename T, typename U, typename... Ts>
struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {
};

template <typename T, typename... Ts>
struct has_type<T, std::tuple<T, Ts...>> : std::true_type {
};

template <class... Arch> class ArcheRegistry
{
  static constexpr std::tuple<Arch...> m_archetypes;
  friend class Scene;
  ArcheRegistry() = default;
  // ---------------------------------------------------- //
  // add and remove
  // ---------------------------------------------------- //

  // Add an entity with N different components, passing N arguments
  template <typename... Components, typename... Args>
  Tuple<Components &...> addComponents(Entity entity, Args &&...args)
  {
    // https://en.cppreference.com/w/cpp/language/fold
    static_assert(sizeof...(Components) == sizeof...(Args),
                  "Number of components and arguments must match.");
    UnsortedArchetype<Components...> &archetype = getArchetype<Components...>();
    (archetype.template pushComponent<Components>(std::forward<Args>(args)),
     ...);
    return archetype.template extractColumn<Components...>(entity);
  }
  // TODO: Add component should move entity to different archetype
  template <typename T, typename... Args>
  T &addComponent(Entity entity, Args &&...args)
  {
    UnsortedArchetype<T> &archetype = getArchetype<T>();
    archetype.template pushComponent<T>(std::forward<Args>(args)...);
    return archetype.template extractComponent<T>(entity);
  }

  // ---------------------------------------------------- //
  // get
  // ---------------------------------------------------- //

  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... Components>
  Tuple<Components &...> &getComponents(Entity entity)
  {
    UnsortedArchetype<Components...> &archetype = getArchetype<Components...>();
    return archetype.template extractColumn<Components...>(entity);
  }
  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... Components>
  const Tuple<Components &...> &getComponents(Entity entity) const
  {
    UnsortedArchetype<Components...> &archetype = getArchetype<Components...>();
    return archetype.template extractColumn<Components...>(entity);
  }
  // A component of an entity
  template <typename T> T &getComponent(Entity entity)
  {
    UnsortedArchetype<T> &archetype = getArchetype<T>();
    return archetype.template extractComponent<T>(entity);
  }
  // A component of an entity
  template <typename T> const T &getComponent(Entity entity) const
  {
    UnsortedArchetype<T> &archetype = getArchetype<T>();
    return archetype.template extractComponent<T>(entity);
  }

  template <typename... Components> bool has(Entity entity) const
  {
    UnsortedArchetype<Components...> &arch = getArchetype<Components...>();
    return arch.has(entity);
  }

  /* Remove a single column of an archetype, which represents the components
  of an entity. In this function, the components have to be provided to remove
  the entity, allowing it to remove faster. For a more generic and slower
  function, call without providing the components If entity is found, then 0 is
  returned, otherwise 0 is returned*/
  template <typename... Components> void remove(Entity entity)
  {
    UnsortedArchetype<Components...> &archetype = getArchetype<Components...>();
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
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(createIteratorFromComponentBegin<Components>()...);
  }
  // return tuple of iterators
  template <typename... Components>
  const Tuple<reg::Iterator<Components>...> end() const
  {
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(
        createIteratorFromComponentEnd<Components..., Components>()...);
  }

  // Extract all vectors of that particular component from the archetypes
  template <typename... Components, typename ParticularComponent>
  reg::Iterator<ParticularComponent> createIteratorFromComponentBegin() const
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    std::vector<const std::vector<Entity> *> entities;
    entities.reserve(m_archetypes.size());

    // https://stackoverflow.com/questions/54640419/iterating-over-tuple-in-c17-20
    std::apply(
        [&](const auto &...archetype) {
          (vectors.push_back(archetype.getComponent()), ...);
          (entities.push_back(archetype.m_entities), ...);
        },
        m_archetypes);

    return reg::Iterator(vectors, 0, 0, entities);
  }

  template <typename... Components, typename ParticularComponent>
  reg::Iterator<ParticularComponent> createIterateFromComponentEnd() const
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    std::vector<const std::vector<Entity> *> entities;
    entities.reserve(m_archetypes.size());

    // https://stackoverflow.com/questions/54640419/iterating-over-tuple-in-c17-20
    std::apply(
        [&](const auto &...archetype) {
          (vectors.push_back(archetype.getComponent()), ...);
          (entities.push_back(archetype.m_entities), ...);
        },
        m_archetypes);

    return reg::Iterator(vectors, vectors.size(), 0, entities);
  }

  // ---------------------------------------------------- //
  // get Maps
  // ---------------------------------------------------- //

  template <typename... Components>
  UnsortedArchetype<Components...> &getArchetype()
  {
    static_assert(has_type<Archetype<Components...>, std::tuple<Arch...>>::type,
                  "The registry doesn't have this type of archetype, are you "
                  "sure you initialize the Registry with all Archetypes");
    return std::get<UnsortedArchetype<Components...>>(m_archetypes);
  }

  template <typename... Cs, typename... Ts>
  constexpr bool hasAllComponents(std::tuple<Ts...>)
  {
    return (contains<Cs>(std::declval<std::tuple<Ts...>>()) && ...);
  }

  template <typename U, typename... T> constexpr bool contains(std::tuple<T...>)
  {
    return (std::is_same_v<U, T> || ...);
  }
  // // Filter archetypes to find those that have at least the components in the
  // // bitmask
  // template <typename FilterTuple, typename... A>
  // constexpr std::tuple<A...> filterArchetypes(std::tuple<A...> archetypes)
  // {
  //   return std::tuple_cat((hasAllComponents<FilterTuple>(archetypes)
  //                              ? std::tuple<A>{}
  //                              : std::tuple<>{})...);
  // }
};
} // namespace pain
