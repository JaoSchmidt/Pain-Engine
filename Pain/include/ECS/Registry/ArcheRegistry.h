#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/IteratorArchetype.h"
#include "pch.h"

#include "Core.h"

#include "ECS/Entity.h"
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace pain
{

class ArcheRegistry
{
  std::map<int, IUnsortedArchetype *> m_archetypes;
  std::map<std::type_index, int> m_componentIDs;
  std::map<int, std::type_index> m_archetypesIDs;
  // std::vector<void *> m_archetypes;
  int count = -1;
  friend class Scene;
  ArcheRegistry() = default;
  // ---------------------------------------------------- //
  // get Maps
  // ---------------------------------------------------- //
  template <typename... Cs, typename... Ts>
  constexpr bool hasAllComponents(UnsortedArchetype<Ts...>)
  {
    return (contains<Cs>(std::declval<UnsortedArchetype<Ts...>>()) && ...);
  }
  template <typename U, typename... T>
  constexpr bool contains(UnsortedArchetype<T...>)
  {
    return (std::is_same_v<U, T> || ...);
  }
  int createBitMaskId() { return 1 << ++count; }

  // will check if component has associated bitmask, otherwise will create
  template <typename Component> int getComponentId()
  {
    auto it = m_componentIDs.find(std::type_index(typeid(Component)));
    if (it != m_componentIDs.end()) {
      m_componentIDs.at(std::type_index(typeid(Component)));
    } else {
      m_componentIDs.emplace(std::type_index(typeid(Component)),
                             createBitMaskId());
    }
  }

  template <typename Component> int getComponentBitMask()
  {
    return m_componentIDs.at(std::type_index(typeid(Component)));
  }

  template <typename... Components> int getBitMask()
  {
    return (getComponentBitMask<Components>() | ...);
  }

  template <typename... Components> Archetype<Components...> &filterComponents()
  {

    return m_archetypes.at(getArchetypeId<Components...>());
  }

  template <typename... Components> Archetype<Components...> &getArchetype()
  {

    return m_archetypes.at(getArchetypeId<Components...>());
  }
  // Extract all vectors of that particular component from the archetypes
  template <typename... Components, typename ParticularComponent>
  reg::Iterator<ParticularComponent> createIteratorFromComponentBegin() const
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    std::vector<const std::vector<Entity> *> entities;
    entities.reserve(m_archetypes.size());

    std::apply(
        [&](const auto &...archetype) {
          (vectors.push_back(archetype.getComponent()), ...);
          (entities.push_back(archetype.m_entities), ...);
        },
        m_archetypes);

    return reg::Iterator(vectors, 0, 0, entities);
  }

  template <typename... Components, typename ParticularComponent,
            typename... Ts>
  void test(Archetype<Ts...> &arch,
            std::vector<std::vector<ParticularComponent> *> &vectors,
            std::vector<const std::vector<Entity> *> &entities)
  {
    if (hasAllComponents<Components...>(std::declval<Archetype<Ts...>>)) {
      vectors.push_back(arch.getComponent());
      entities.push_back(arch.m_entities);
    }
  }

  template <typename ParticularComponent>
  reg::Iterator<ParticularComponent>
  createIterateFromComponentEnd(int bitMask) const
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    std::vector<const std::vector<Entity> *> entities;
    entities.reserve(m_archetypes.size());

    for (const auto &iArchetype : m_archetypes) {
      if ((iArchetype.first & bitMask) == bitMask) {
        if (auto *compVec =
                iArchetype.second->getComponent<ParticularComponent>()) {
          vectors.push_back(compVec);
          entities.push_back(&iArchetype.second->m_entities);
        }
      }
    }
    return reg::Iterator(vectors, vectors.size(), 0, entities);
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
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(
        createIteratorFromComponentBegin<Components>(bitMask)...);
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
};
} // namespace pain
