#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/IteratorArchetype.h"

#include <typeindex>

namespace pain
{

template <typename... Ts>
concept IsNoneType = (sizeof...(Ts) == 0);

template <typename... Ts>
concept IsSingleType = (sizeof...(Ts) == 1);

template <typename... Ts>
concept IsMultipleTypes = (sizeof...(Ts) > 1);

static int id = 0;
class ArcheRegistry
{
public:
  int getid() const { return id; }

private:
  std::map<int, Archetype> m_archetypes = {};
  friend struct System;
  friend class Scene;
  ArcheRegistry() : m_archetypes() { id = id + 1; };

  // ---------------------------------------------------- //
  // add
  // ---------------------------------------------------- //
  // TODO: (maybe?) Make possible to add new components to aleardy existing
  // entities. Therefore, any added component should move the entity a
  // different archetype

  // Add an entity with N different components, passing N arguments
  // return a tuple with its components
  template <typename... Components>
  std::tuple<Components &...> createComponents(Entity entity,
                                               Components &&...comps)
  {
    int bitMask = ComponentManager::multiComponentBitmask<Components...>();
    Archetype &archetype = m_archetypes[bitMask];
    // for iterate every component
    (archetype.pushComponent<Components>(std::forward<Components>(comps)), ...);
    archetype.addEntity(entity);

    return archetype.extractColumn<Components...>(entity);
  }

  // Add an entity with 1 component
  // return the component
  template <typename Component>
  Component &createComponent(Entity entity, Component &&args)
  {
    int bitMask = ComponentManager::singleComponentBitmask<Component>();
    Archetype &archetype = m_archetypes[bitMask];
    // for iterate every component
    archetype.pushComponent<Component>(std::forward<Component>(args));
    archetype.addEntity(entity);

    return archetype.extractComponent<Component>(entity);
  }

  /* TODO: In theory, we can call createComponents with "ArgTuples" instead of
     the above, this will be it possible to, instead of calling this:
     createComponents(
        MovementComponent{},
        RotationComponent{},
        TransformComponent{},
        OrthoCameraComponent{aspectRatio, zoomLevel},
        NativeScriptComponent{"Camera nsc"}
      );
     // We can call this:
     createComponents(
          std::make_tuple(),
          std::make_tuple(),
          std::make_tuple(),
          std::make_tuple(aspectRatio, zoomLevel),
          std::make_tuple("Camera nsc")
      );
      In theory, this can be done to make it a little more cheaper to insert
     components, as they will be emplaced direclty inside the vector instead of
     being moved to
  */

  // template <typename... Components, typename... ArgTuples>
  // std::tuple<Components &...> createComponents(Entity entity,
  //                                              ArgTuples &&...args)
  // {
  //   static_assert(sizeof...(Components) == sizeof...(ArgTuples),
  //                 "Mismatch in components and constructor arguments");

  //   int bitMask = createBitMasks<Components...>();
  //   Archetype &archetype = m_archetypes[bitMask];

  //   // for iterate every component
  //   (archetype.pushComponent<Components>(std::forward<ArgTuples>(args)),
  //   ...); return archetype.extractColumn<Components...>(entity);
  // }
  // template <typename Component, typename Tuple>
  // void pushComponentHelper(Archetype &archetype, Tuple &&args)
  // {
  //   std::apply(
  //       [&](auto &&...unpackedArgs) {
  //         archetype.pushComponent<Component>(
  //             std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
  //       },
  //       std::forward<Tuple>(args));
  // }
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
    return reg::Iterator<ParticularComponent>(std::move(vectors), 0, 0,
                                              std::move(entities));
  }

  template <typename ParticularComponent>
  reg::Iterator<ParticularComponent> createIteratorFromComponentEnd(int bitMask)
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    std::vector<std::vector<Entity> *> entities;
    entities.reserve(m_archetypes.size());

    // TODO: It is possible to cache this result so it doesn't have to be made
    // every time
    for (auto &pair : m_archetypes) {
      if ((pair.first & bitMask) == bitMask) {
        Archetype &archetype = pair.second;
        std::vector<ParticularComponent> *v =
            &archetype.getComponent<ParticularComponent>();
        vectors.push_back(v);
        entities.push_back(&archetype.m_entities);
      }
    }
    const int size =
        vectors.size(); // attention to evaluation order, do not replace below
    return reg::Iterator<ParticularComponent>(std::move(vectors), size, 0,
                                              std::move(entities));
  }

  // return iterator of component

  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> begin()
  {
    int bitMask = ComponentManager::singleComponentBitmask<T>();
    return createIteratorFromComponentBegin<T>(bitMask);
  }

  // return iterator of component
  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> end()
  {
    int bitMask = ComponentManager::singleComponentBitmask<T>();
    return createIteratorFromComponentEnd<T>(bitMask);
  }

  // return tuple of iterators
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> begin()
  {
    int bitMask = ComponentManager::multiComponentBitmask<Components...>();
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
    int bitMask = ComponentManager::multiComponentBitmask<Components...>();
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(
        createIteratorFromComponentEnd<Components>(bitMask)...);
  }

  // ---------------------------------------------------- //
  // get
  // ---------------------------------------------------- //
public:
  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... TargetComponents>
  std::tuple<TargetComponents &...> getComponents(Entity entity,
                                                  int archetypeBitmask)
  {
    Archetype &archetype = m_archetypes.at(archetypeBitmask);
    return archetype.extractColumn<TargetComponents...>(entity);
  }
  // Get a single column of an archetype as a tuple, which represents the
  // components of an entity.
  template <typename... TargetComponents>
  const std::tuple<TargetComponents &...>
  getComponents(Entity entity, int archetypeBitmask) const
  {
    const Archetype &archetype = m_archetypes.at(archetypeBitmask);
    return archetype.extractColumn<TargetComponents...>(entity);
  }
  // A single component of an entity, no tuple
  template <typename T> T &getComponent(Entity entity, int bitmask)
  {
    Archetype &archetype = m_archetypes.at(bitmask);
    return archetype.extractComponent<T>(entity);
  }
  // A single component of an entity, no tuple
  template <typename T> const T &getComponent(Entity entity, int bitmask) const
  {
    const Archetype &archetype = m_archetypes.at(bitmask);
    return archetype.extractComponent<T>(entity);
  }

private:
  // ---------------------------------------------------- //
  // has
  // ---------------------------------------------------- //

  // Are all components inside the targetBitMask?
  template <typename... ObjectComponents>
  constexpr bool containsAll(int targetBitMask) const
  {
    int objectBitMask =
        ComponentManager::multiComponentBitmask<ObjectComponents...>();
    return (targetBitMask | objectBitMask) == objectBitMask;
  }
  // Targeted has at least one component that a specific archetype also has
  template <typename... TargetComponents>
  constexpr bool hasAny(int specificBitMask) const
  {
    if constexpr (sizeof...(TargetComponents) == 1) {
      int targetBitMask =
          ComponentManager::singleComponentBitmask<TargetComponents...>();
      return (targetBitMask & specificBitMask) != 0;
    } else {
      int targetBitMask =
          ComponentManager::multiComponentBitmask<TargetComponents...>();
      return (targetBitMask & specificBitMask) != 0;
    }
  }

  // ---------------------------------------------------- //
  // remove
  // ---------------------------------------------------- //

  // Remove the entity, alongside its components from its archetype
  template <typename... ObjectComponents>
  bool remove(Entity entity, int bitmask)
  {
    Archetype &archetype = m_archetypes.at(bitmask);
    return archetype.remove<ObjectComponents...>(entity);
  }
  // Remove the components from an entity
  // template <typename... TargetComponents, typename... ObjectComponents>
  // bool remove(Entity entity)
  // {
  //   int bitMask = getBitMask<ObjectComponents...>();
  //   Archetype &archetype = m_archetypes.at(bitMask);
  //   int targetBitMask = createBitMask<TargetComponents...>();
  //   Archetype &targetArchetype = m_archetypes.at(targetBitMask);
  //   return archetype.removeComponents<TargetComponents...>(entity,
  //                                                          targetArchetype);
  // }
};
} // namespace pain
