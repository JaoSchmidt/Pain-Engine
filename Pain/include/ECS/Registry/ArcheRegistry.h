#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/ExcludeComponents.h"
#include "ECS/Registry/IteratorArchetype.h"

#include <queue>
#include <typeindex>

namespace reg
{

template <typename... Ts>
concept IsNoneType = (sizeof...(Ts) == 0);

template <typename... Ts>
concept IsSingleType = (sizeof...(Ts) == 1);

template <typename... Ts>
concept IsMultipleTypes = (sizeof...(Ts) > 1);

template <typename ComponentManagerT> class ArcheRegistry
{
  static_assert(is_compile_time_bitmask_v<ComponentManagerT>,
                "ArcheRegistry requires ComponentManagerT to be a "
                "reg::CompileTimeBitMask<...> type");

  std::map<Bitmask, Archetype> m_archetypes = {};
  std::vector<Record> m_entities;
  std::queue<reg::Entity> m_availableEntities = {};
  std::int32_t numberOfEntities = -1;

public:
  ArcheRegistry() : m_archetypes() {};
  Entity createEntity()
  {
    reg::Entity id;
    if (m_availableEntities.empty()) {
      id = reg::Entity(++numberOfEntities);
      addRecord(Bitmask{-1}, Column{-1});
    } else {
      id = m_availableEntities.front();
      m_availableEntities.pop();
    }
    return id;
  }

  // ---------------------------------------------------- //
  // create
  // ---------------------------------------------------- //

  // Add an entity with N different components, passing using N arguments
  // This is useful to create all components at once, instead of one by one with
  // createComponent (singular)
  template <typename... Components>
  std::tuple<Components &...> createComponents(Entity entity,
                                               Components &&...comps)
  {
    Bitmask bitMask = getMultipleBitmask<Components...>();
    P_ASSERT(m_entities.at((size_t)entity).bitmask == bitMask ||
                 m_entities.at((size_t)entity).bitmask == -1,
             "Component reset isn't implemented yet. TODO: need to remove "
             "component of old archetype");
    Archetype &archetype =
        m_archetypes[bitMask]; // this will create an empty map
    Column column = Column{-1};
    (...,
     (column = archetype.pushComponent<Components>(std::forward<Components>(
          comps)))); // and this should create its columns
    updateRecord(entity, bitMask, column);

    return std::tie<Components &...>(
        archetype.fetchComponent<Components>(column)...);
  }

  // use this in case you want to create a single component for an entity, not
  // ideal in most cases
  template <typename Component>
  Component &createComponent(Entity entity, Component &&args)
  {
    Bitmask bitMask = getSingleBitmask<Component>();
    P_ASSERT(m_entities.at(entity).bitmask == bitMask ||
                 m_entities.at((size_t)entity).bitmask == -1,
             "Component reset isn't implemented yet. TODO: need to remove "
             "component of old archetype");
    Archetype &archetype = m_archetypes[bitMask];
    size_t column =
        archetype.pushComponent<Component>(std::forward<Component>(args));
    updateRecord(entity, bitMask, column);

    return archetype.fetchComponent<Component>(column);
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
  // add
  // ---------------------------------------------------- //

  // Expand the components of an entity, changing its "type". This is to make
  // the objects more flexible but can also be used as a feature of the engine.
  // Slow process because we are also moving all its N components to N new
  // vectors
  template <typename... Components>
  std::tuple<Components &...> addComponents(Entity entity,
                                            Components &&...comps)
  {
    Bitmask oldBitmask = m_entities[static_cast<unsigned>(entity)].bitmask;
    int oldColumn = m_entities[static_cast<unsigned>(entity)].column;
    Bitmask newBitMask = getMultipleBitmask<Components...>();
    Archetype &archetype = m_archetypes[oldBitmask];
    Archetype &newArchetype = m_archetypes[newBitMask];

    Column newColumn =
        moveEntity<Components...>(archetype, newArchetype, entity, newBitMask,
                                  oldColumn, std::forward(comps)...);
    return std::tie<Components &...>(
        archetype.fetchComponent<Components>(newColumn)...);
  }

private:
  template <typename... Components>
  Column moveEntity(Archetype &from, Archetype &to, Entity entity,
                    Bitmask newBitMask, Column oldColumn, Components &&...comps)
  {
    std::tuple<Components &...> oldComponents =
        from.extractColumn<Components...>(oldColumn);

    Column newColumnIndex;
    // add old ones
    std::apply(
        [&](auto &...oldComps) {
          newColumnIndex = (to.pushComponent<std::decay_t<decltype(oldComps)>>(
                                std::move(oldComps)),
                            ...);
        },
        oldComponents);

    // add new ones
    (..., to.pushComponent<Components>(std::forward<Components>(comps)));

    m_entities[static_cast<unsigned>(entity)].bitmask = newBitMask;
    m_entities[static_cast<unsigned>(entity)].column = newColumnIndex;
    from.remove<Components...>(oldColumn);
    return newColumnIndex;
  }

  // ==================================================== //
  // iterate
  // ==================================================== //

  // Extract all vectors of that particular component from the archetypes
  template <typename ParticularComponent>
  reg::Iterator<ParticularComponent>
  createBeginIteratorFromComponent(int bitMask, int excludeBitMask)
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    for (auto &pair : m_archetypes) {
      if (((pair.first & bitMask) == bitMask) &&
          (pair.first & excludeBitMask) == 0) {
        Archetype &archetype = pair.second;
        std::vector<ParticularComponent> *v =
            &archetype.getComponent<ParticularComponent>();
        vectors.push_back(v);
        // vectors.push_back(&archetype.getComponent<ParticularComponent>());
      }
    }
    return reg::Iterator<ParticularComponent>(std::move(vectors), 0, 0);
  }

  template <typename ParticularComponent>
  reg::Iterator<ParticularComponent>
  createEndIteratorFromComponent(int bitMask, int excludeBitMask)
  {
    std::vector<std::vector<ParticularComponent> *> vectors;
    vectors.reserve(m_archetypes.size());

    // TODO: It is possible to cache this result so it doesn't have to be made
    // every time, altough it is made very rarely
    for (auto &pair : m_archetypes) {
      if (((pair.first & bitMask) == bitMask) &&
          (pair.first & excludeBitMask) == 0) {
        Archetype &archetype = pair.second;
        std::vector<ParticularComponent> *v =
            &archetype.getComponent<ParticularComponent>();
        vectors.push_back(v);
      }
    }
    // attention to evaluation order, do not replace below, will get UB
    const size_t size = vectors.size();
    return reg::Iterator<ParticularComponent>(std::move(vectors), size, 0);
  }

  template <typename FirstComponent, typename... Rest>
  size_t sizeOfComponent(int bitMask, int excludeBitMask)
  {
    size_t totalSize = 0;
    // TODO: It is possible to cache this result so it doesn't have to be made
    // every time, altough it is made very rarely
    for (auto &pair : m_archetypes) {
      if (((pair.first & bitMask) == bitMask) &&
          (pair.first & excludeBitMask) == 0) {
        Archetype &archetype = pair.second;
        const std::vector<FirstComponent> *v =
            &archetype.getComponent<FirstComponent>();
        totalSize += v->size();
      }
    }
    return totalSize;
  }

public:
  // return iterator of component

  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> begin()
  {
    int bitMask = getSingleBitmask<T>();
    return createBeginIteratorFromComponent<T>(bitMask, 0);
  }

  // return iterator of component
  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> end()
  {
    int bitMask = getSingleBitmask<T>();
    return createEndIteratorFromComponent<T>(bitMask, 0);
  }

  // return tuple of iterators
  template <typename... Components, typename... ExcludeComponents>
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...>
  begin(exclude_t<ExcludeComponents...> = {})
  {
    int bitMask = getMultipleBitmask<Components...>();
    int excludeBitMask = getMultipleBitmask<ExcludeComponents...>();
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(createBeginIteratorFromComponent<Components>(
        bitMask, excludeBitMask)...);
  }
  // return tuple of iterators
  template <typename... Components, typename... ExcludeComponents>
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...>
  end(exclude_t<ExcludeComponents...> = {})
  {
    int bitMask = getMultipleBitmask<Components...>();
    int excludeBitMask = getMultipleBitmask<ExcludeComponents...>();
    // for each component, extract all vectors of that component in each
    // Archetype (SoA)
    // return into a tuple of components
    return std::make_tuple(
        createEndIteratorFromComponent<Components>(bitMask, excludeBitMask)...);
  }
  // ==================================================== //
  // size
  // ==================================================== //

  // return the size of the iterators
  template <typename... Components, typename... ExcludeComponents>
    requires IsMultipleTypes<Components...>
  size_t iteratorSize(exclude_t<ExcludeComponents...> = {})
  {
    int bitMask = getMultipleBitmask<Components...>();
    int excludeBitMask = getMultipleBitmask<ExcludeComponents...>();
    // NOTE: altough all components are necessary for the bitmasks, we only need
    // to see one for the size
    return sizeOfComponent<Components...>(bitMask, excludeBitMask);
  }

  // ==================================================== //
  // get
  // ==================================================== //
  template <typename... TargetComponents>
  std::tuple<TargetComponents &...> getComponents(Entity entity)
  {
    static_assert(hasAllBitmask<TargetComponents...>(),
                  "When using getComponent some component didn't exist inside  "
                  "the CompileTimeBitMask");
    Archetype &archetype =
        m_archetypes.at(m_entities[static_cast<unsigned>(entity)].bitmask);
    return archetype.extractColumn<TargetComponents...>(
        m_entities[static_cast<unsigned>(entity)].column);
  }
  template <typename... TargetComponents>
  const std::tuple<TargetComponents &...> getComponents(Entity entity) const
  {
    static_assert(hasAllBitmask<TargetComponents...>(),
                  "When using getComponent some component didn't exist inside "
                  "the CompileTimeBitMask");
    const Archetype &archetype =
        m_archetypes.at(m_entities[static_cast<unsigned>(entity)].bitmask);
    return archetype.extractColumn<TargetComponents...>(
        m_entities[static_cast<unsigned>(entity)].column);
  }
  template <typename T> T &getComponent(Entity entity)
  {
    Archetype &archetype =
        m_archetypes.at(m_entities[static_cast<unsigned>(entity)].bitmask);
    return archetype.fetchComponent<T>(
        m_entities[static_cast<unsigned>(entity)].column);
  }
  template <typename T> const T &getComponent(Entity entity) const
  {
    const Archetype &archetype =
        m_archetypes.at(m_entities[static_cast<unsigned>(entity)].bitmask);
    return archetype.fetchComponent<T>(
        m_entities[static_cast<unsigned>(entity)].column);
  }

  // ==================================================== //
  // has
  // ==================================================== //

  // Are all components inside the targetBitMask?
  template <typename... ObjectComponents>
  constexpr bool containsAll(Entity entity) const
  {
    Bitmask targetBitMask = m_entities[static_cast<unsigned>(entity)].bitmask;
    int objectBitMask = getMultipleBitmask<ObjectComponents...>();
    return (targetBitMask | objectBitMask) == objectBitMask;
  }
  // Targeted has at least one component that a specific archetype also has
  template <typename... TargetComponents> bool hasAny(Entity entity) const
  {
    Bitmask specificBitMask = m_entities[static_cast<unsigned>(entity)].bitmask;
    if constexpr (sizeof...(TargetComponents) == 1) {
      int targetBitMask = getSingleBitmask<TargetComponents...>();
      return (targetBitMask & specificBitMask) != 0;
    } else {
      int targetBitMask = getMultipleBitmask<TargetComponents...>();
      return (targetBitMask & specificBitMask) != 0;
    }
  }

  // ==================================================== //
  // remove
  // ==================================================== //

  // Removal of a single entity can be O(n) in worst case (because we need to
  // update the last element's Record). If possible, prefer to batch
  template <typename... ObjectComponents> bool remove(Entity entity)
  {
    Bitmask targetBitMask = m_entities[static_cast<unsigned>(entity)].bitmask;
    Column targetColumn = m_entities[static_cast<unsigned>(entity)].column;
    Archetype &archetype = m_archetypes.at(targetBitMask);
    int lastEntityColumn = archetype.remove<ObjectComponents...>(targetColumn);
    removeEntity(entity);
    for (Record &record : m_entities) {
      if (record.bitmask == targetBitMask &&
          record.column == lastEntityColumn) {
        record.column = targetColumn;
        return true;
      }
    }
    return false;
  }
  template <typename... ObjectComponents>
  void removeBatch(std::span<const Entity> entities)
  {
    // useful
    struct RecordHash {
      std::size_t operator()(const Record &r) const noexcept
      {
        return std::hash<Bitmask>()(r.bitmask) ^
               (std::hash<Column>()(r.column) << 1);
      }
    };
    struct RecordEq {
      bool operator()(const Record &a, const Record &b) const noexcept
      {
        return a.bitmask == b.bitmask && a.column == b.column;
      }
    };

    if (entities.empty())
      return;

    std::unordered_set<Record, RecordHash, RecordEq> replacedMap;
    replacedMap.reserve(entities.size());

    // remove NOTE: for high remove batches, it may be faster to cache
    // archetypes
    for (auto &target : entities) {
      Bitmask &bitmask = m_entities[target].bitmask;
      Column &column = m_entities[target].column;
      Archetype &archetype = m_archetypes.at(bitmask);
      replacedMap.emplace(bitmask,
                          archetype.remove<ObjectComponents...>(column));
      bitmask = Bitmask{-1};
      column = Column{-1};
    }
    // update remaining records columns
    std::size_t updated = 0;
    for (Record &record : m_entities) {
      // NOTE: there is a possibility that it will find the some invalid record,
      // in this case, we need to test it to make sure that is the correct
      // behaviour

      P_ASSERT(record.bitmask != Bitmask{-1} && record.column != Column{-1},
               "The debug begins");
      if (auto it = replacedMap.find(record); it != replacedMap.end()) {
        record.column = it->second;
        if (++updated == replacedMap.size())
          break;
      }
    }
  }

private:
  void removeEntity(Entity entity)
  {
    m_entities[static_cast<unsigned>(entity)].bitmask = Bitmask{0};
    m_entities[static_cast<unsigned>(entity)].column = Column{0};
    m_availableEntities.push(entity);
  }
  template <typename... Components>
  static constexpr Bitmask getMultipleBitmask()
  {
    return ComponentManagerT::template multiComponentBitmask<Components...>();
  }
  template <typename Component> static constexpr Bitmask getSingleBitmask()
  {
    return ComponentManagerT::template singleComponentBitmask<Component>();
  }
  template <typename... Component> static constexpr bool hasAllBitmask()
  {
    return ComponentManagerT::template allRegistered<Component...>();
  }
  void addRecord(Bitmask bitmask, Column column)
  {
    m_entities.emplace_back(bitmask, column);
  }
  void addRecord(Bitmask bitmask, size_t column)
  {
    m_entities.emplace_back(bitmask, Column{static_cast<int32_t>(column)});
  }
  void updateRecord(Entity entity, Bitmask bitmask, Column column)
  {
    m_entities[static_cast<unsigned>(entity)] = {bitmask, column};
  }
  void updateRecord(Entity entity, Bitmask bitmask, size_t column)
  {
    m_entities[static_cast<unsigned>(entity)] = {
        bitmask, Column{static_cast<int32_t>(column)}};
  }
};
} // namespace reg
