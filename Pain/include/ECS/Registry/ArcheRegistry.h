#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/IteratorArchetype.h"

#include <bitset>
#include <iostream>
#include <typeindex>

namespace pain
{

template <typename... Ts>
concept IsSingleType = (sizeof...(Ts) == 1);

template <typename... Ts>
concept IsMultipleTypes = (sizeof...(Ts) > 1);

class ArcheRegistry
{
  std::map<int, Archetype> m_archetypes = {};
  std::map<std::type_index, int> m_componentBitMasks = {};
  int count = -1;
  friend class Scene;
  ArcheRegistry() = default;

  // ---------------------------------------------------- //
  // manipulation of maps
  // ---------------------------------------------------- //

  int createBitMaskId() { return 1 << ++count; }

  // will check if component has associated bitmask, otherwise will create
  template <typename Component> int createBitMask()
  {
    auto it = m_componentBitMasks.find(std::type_index(typeid(Component)));
    if (it != m_componentBitMasks.end()) {
      return m_componentBitMasks.at(std::type_index(typeid(Component)));
    } else {
      auto it = m_componentBitMasks.emplace(std::type_index(typeid(Component)),
                                            createBitMaskId());
      P_ASSERT(it.second, "Could not create new component bitMask");
      PLOG_I("New component bitmask added {}", it.first->second);
      return it.first->second;
    }
  }
  template <typename... Components> int createBitMasks()
  {
    return (0 | ... | createBitMask<Components>());
  }

  template <typename Component> int getComponentBitMask() const
  {
    return m_componentBitMasks.at(std::type_index(typeid(Component)));
  }
  // Get the correct bitmask corresponding to the archetype components
  template <typename... Components> int getBitMask() const
  {
    return (0 | ... | getComponentBitMask<Components>());
  }

  // ---------------------------------------------------- //
  // iterate
  // ---------------------------------------------------- //

  // Extract all vectors of that particular component from the archetypes
  template <typename ParticularComponent>
  reg::Iterator<ParticularComponent>
  createIteratorFromComponentBegin(int bitMask)
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    std::vector<std::vector<Entity> *> entities;
    entities.reserve(m_archetypes.size());

    for (auto &pair : m_archetypes) {
      if ((pair.first & bitMask) == bitMask) {
        Archetype &archetype = pair.second;
        std::vector<ParticularComponent> *v =
            &archetype.getComponent<ParticularComponent>();
        vectors.push_back(v);
        // vectors.push_back(&archetype.getComponent<ParticularComponent>());
        entities.push_back(&archetype.m_entities);
      }
    }
    return reg::Iterator<ParticularComponent>(vectors, 0, 0, entities);
  }

  template <typename ParticularComponent>
  reg::Iterator<ParticularComponent> createIteratorFromComponentEnd(int bitMask)
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    std::vector<std::vector<Entity> *> entities;
    entities.reserve(m_archetypes.size());

    for (auto &pair : m_archetypes) {
      if ((pair.first & bitMask) == bitMask) {
        Archetype &archetype = pair.second;
        std::vector<ParticularComponent> *v =
            &archetype.getComponent<ParticularComponent>();
        vectors.push_back(v);
        entities.push_back(&archetype.m_entities);
      }
    }
    return reg::Iterator<ParticularComponent>(vectors, vectors.size(), 0,
                                              entities);
  }

  // return iterator of component

  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> begin()
  {
    int bitMask = getBitMask<T>();
    return createIteratorFromComponentBegin<T>(bitMask);
  }

  // return iterator of component
  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> end()
  {
    int bitMask = getBitMask<T>();
    return createIteratorFromComponentEnd<T>(bitMask);
  }

  // return tuple of iterators
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> begin()
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
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> end()
  {
    int bitMask = getBitMask<Components...>();
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(
        createIteratorFromComponentEnd<Components>(bitMask)...);
  }

  // ---------------------------------------------------- //
  // add
  // ---------------------------------------------------- //

  // Add an entity with N different components, passing N arguments
  // return a tuple with its components
  template <typename... Components>
  std::tuple<Components &...> addComponents(Entity entity, Components &&...args)
  {
    int bitMask = createBitMasks<Components...>();
    auto it = m_archetypes.find(bitMask);
    if (it == m_archetypes.end()) {
      std::bitset<8> x(bitMask);
      std::cout << "New bitmask added " << x << std::endl;
    }
    Archetype &archetype = m_archetypes[bitMask];
    // for iterate every component
    (archetype.pushComponent<Components>(std::forward<Components>(args)), ...);
    archetype.addEntity(entity);

    return archetype.extractColumn<Components...>(entity);
  }
  // TODO: Add component should move entity to different archetype

  // Add an entity with 1 component
  // return the component
  template <typename Component>
  Component &addComponent(Entity entity, Component &&args)
  {
    int bitMask = createBitMask<Component>();
    auto it = m_archetypes.find(bitMask);
    if (it == m_archetypes.end()) {
      std::bitset<8> x(bitMask);
      std::cout << "New bitmask added " << x << std::endl;
    }
    Archetype &archetype = m_archetypes[bitMask];
    // for iterate every component
    archetype.pushComponent<Component>(std::forward<Component>(args));
    archetype.addEntity(entity);

    return archetype.extractComponent<Component>(entity);
  }

  // ---------------------------------------------------- //
  // get
  // ---------------------------------------------------- //

  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... Components>
  std::tuple<Components &...> getAllComponents(Entity entity)
  {
    int bitMask = getBitMask<Components...>();
    Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.extractColumn<Components...>(entity);
  }
  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... Components>
  const std::tuple<Components &...> getAllComponents(Entity entity) const
  {
    int bitMask = getBitMask<Components...>();
    const Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.extractColumn<Components...>(entity);
  }

  // A single component of an entity
  template <typename T, typename... Components> T &getComponent(Entity entity)
  {
    int bitMask = getBitMask<Components...>();
    Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.extractComponent<T>(entity);
  }
  // A single component of an entity
  template <typename T, typename... Components>
  const T &getComponent(Entity entity) const
  {
    int bitMask = getBitMask<Components...>();
    const Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.extractComponent<T>(entity);
  }

  // ---------------------------------------------------- //
  // has
  // ---------------------------------------------------- //

  // Targeted has function for a specific archetype
  template <typename... Components> bool has(Entity entity) const
  {
    int bitMask = getBitMask<Components...>();
    const Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.has(entity);
  }
  // Slightly slower "has" function, because it will look on every known
  // archetype
  bool has(Entity entity) const
  {
    for (const auto &pair : m_archetypes) {
      if (pair.second.has(entity))
        return true;
    }
    return false;
  }

  // ---------------------------------------------------- //
  // remove
  // ---------------------------------------------------- //

  // Remove the entity, alongside its components, to the archetype
  template <typename... Components> bool remove(Entity entity)
  {
    int bitMask = getBitMask<Components...>();
    Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.remove<Components...>(entity);
  }
};
} // namespace pain
