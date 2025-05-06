module;
#include "CoreFiles/LogWrapper.h"
export module pain.ArcheRegistry;
import pain.Archetype;
import pain.IteratorArchetype;
import pain.Entity;
import <map>;
import <typeindex>;

export namespace pain
{

template <typename... Ts>
concept IsNoneType = (sizeof...(Ts) == 0);

template <typename... Ts>
concept IsSingleType = (sizeof...(Ts) == 1);

template <typename... Ts>
concept IsMultipleTypes = (sizeof...(Ts) > 1);

struct ArcheRegistry {
  std::map<int, Archetype> m_archetypes = {};
  std::map<std::type_index, int> m_componentBitMasks = {};
  int count = -1;
  // friend class Scene;
  ArcheRegistry() = default;

  // ---------------------------------------------------- //
  // manipulation of maps
  // ---------------------------------------------------- //

  inline int createBitMaskId() { return 1 << ++count; }

  // will check if component has associated bitmask, otherwise will create
  template <typename Component> int createBitMask()
  {
    auto it = m_componentBitMasks.find(std::type_index(typeid(Component)));
    if (it != m_componentBitMasks.end()) {
      return it->second;
    } else {
      auto [newIt, isInserted] = m_componentBitMasks.emplace(
          std::type_index(typeid(Component)), createBitMaskId());
      P_ASSERT(isInserted, "Could not create new component bitMask");
      return newIt->second;
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
    int bitMask = createBitMasks<Components...>();
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
    int bitMask = createBitMask<Component>();
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
    const int bitMask = getBitMask<Components...>();
    static_assert(sizeof...(Components) > 0,
                  "You must provide the entity components in the template, "
                  "perhaps you only provided the target component?");
    Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.extractComponent<T>(entity);
  }
  // A single component of an entity
  template <typename T, typename... Components>
  const T &getComponent(Entity entity) const
  {
    static_assert(sizeof...(Components) > 0,
                  "You must provide the entity components in the template, "
                  "perhaps you only provided the target component?");
    const int bitMask = getBitMask<Components...>();
    const Archetype &archetype = m_archetypes.at(bitMask);
    return archetype.extractComponent<T>(entity);
  }

  // ---------------------------------------------------- //
  // has
  // ---------------------------------------------------- //

  // Targeted has function for a specific archetype
  template <typename... TargetComponents, typename... ObjectComponents>
  bool has(Entity entity) const
  {
    int objectBitMask = getBitMask<ObjectComponents...>();
    int targetBitMask = getBitMask<TargetComponents...>();
    return (targetBitMask | objectBitMask) == objectBitMask;
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

  // Remove the entity, alongside its components from it's archetype
  template <typename... ObjectComponents> bool remove(Entity entity)
  {
    int bitMask = getBitMask<ObjectComponents...>();
    Archetype &archetype = m_archetypes.at(bitMask);
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
