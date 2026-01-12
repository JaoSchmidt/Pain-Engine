#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Archetype.h"
#include "ECS/Registry/Bitmask.h"
#include "ECS/Registry/ExcludeComponents.h"

#include <iostream>
#include <queue>
#include <span>
#include <typeindex>

namespace reg
{

template <typename... Components> struct ChunkView {
  std::tuple<Components *...> arrays = {};
  std::vector<reg::Entity> &entities;
  size_t count = 0;
};
template <typename... Components> struct ChunkViewConst {
  std::tuple<Components *...> arrays = {};
  const std::vector<reg::Entity> &entities;
  size_t count;
};

template <typename... Ts>
concept IsNoneType = (sizeof...(Ts) == 0);

template <typename... Ts>
concept IsSingleType = (sizeof...(Ts) == 1);

template <typename... Ts>
concept IsMultipleTypes = (sizeof...(Ts) > 1);

template <typename ComponentManagerT>
  requires(is_compile_time_bitmask_v<ComponentManagerT>)
class ArcheRegistry
{
  std::map<Bitmask, Archetype> m_archetypes = {};
  std::vector<Record> m_records;
  std::queue<reg::Entity> m_availableEntities = {};
  std::int32_t numberOfEntities = -1;

public:
  ArcheRegistry() : m_archetypes() {};

  Entity createEntity(Bitmask archetype = reg::Bitmask{-1})
  {
    reg::Entity id;
    if (m_availableEntities.empty()) {
      id = reg::Entity(++numberOfEntities);
      addRecord(archetype, Column{-1});
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
  template <ECSComponent... Components>
  std::tuple<Components &...> createComponents(Entity entity,
                                               Components &&...comps)
  {
    Bitmask bitMask = getMultipleBitmask<Components...>();
    P_ASSERT(m_records.at((size_t)entity).bitmask == bitMask ||
                 m_records.at((size_t)entity).bitmask == -1,
             "Component reset isn't implemented yet. TODO: need to remove "
             "component of old archetype");
    Archetype &archetype =
        m_archetypes[bitMask]; // this will create an empty map
    Column column =
        archetype.pushComponents(entity, std::forward<Components>(comps)...);

    updateRecord(entity, bitMask, column);

    return std::tie<Components &...>(
        archetype.fetchComponent<Components>(column)...);
  }

  // use this in case you want to create a single component for an entity, not
  // ideal in most cases
  template <ECSComponent Component>
  Component &createComponent(Entity entity, Component &&args)
  {
    Bitmask bitMask = getSingleBitmask<Component>();
    P_ASSERT(m_records.at(entity).bitmask == bitMask ||
                 m_records.at((size_t)entity).bitmask == -1,
             "Component reset isn't implemented yet. TODO: need to remove "
             "component of old archetype");
    Archetype &archetype = m_archetypes[bitMask];
    size_t column =
        archetype.pushComponent<Component>(std::forward<Component>(args));
    updateRecord(entity, bitMask, column);

    return archetype.fetchComponent<Component>(column);
  }

  // ---------------------------------------------------- //
  // add
  // ---------------------------------------------------- //

  // Expand the components of an entity, changing its "type". This is to make
  // the objects more flexible but can also be used as a feature of the engine.
  // Slow process because we are also moving all its N components to N new
  // vectors
  template <ECSComponent... Components>
  std::tuple<Components &...> addComponents(Entity entity,
                                            Components &&...comps)
  {
    Bitmask oldBitmask = m_records[static_cast<unsigned>(entity)].bitmask;
    int oldColumn = m_records[static_cast<unsigned>(entity)].column;
    Bitmask newBitMask = getMultipleBitmask<Components...>();
    Archetype &archetype = m_archetypes[oldBitmask];
    Archetype &newArchetype = m_archetypes[newBitMask];

    Column newColumn =
        moveEntity<Components...>(archetype, newArchetype, entity, newBitMask,
                                  oldColumn, std::forward(comps)...);
    return std::tie<Components &...>(
        newArchetype.fetchComponent<Components>(newColumn)...);
  }

  // If bitmask is known, you can manually push components into the archetype
  template <ECSComponent C>
  void manualPush(Entity entity, Bitmask bitmask, C &&comps)
  {
    Archetype &archetype = m_archetypes[bitmask];
    Column column = m_records[static_cast<unsigned long>(entity)].column;
    if (column == -1) {
      m_records[static_cast<unsigned long>(entity)].column =
          archetype.pushComponents(entity, std::forward<C>(comps));
    } else {
      archetype.pushComponents(std::forward<C>(comps));
    }
  }

private:
  template <ECSComponent... Components>
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

    m_records[static_cast<unsigned>(entity)].bitmask = newBitMask;
    m_records[static_cast<unsigned>(entity)].column = newColumnIndex;
    from.remove<Components...>(oldColumn);
    return newColumnIndex;
  }

public:
  // ==================================================== //
  // iterate chunks
  // ==================================================== //

  // return vector of chunks
  template <ECSComponent... Components, ECSComponent... ExcludeComponents>
  std::vector<ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    int bitMask = getMultipleBitmask<Components...>();
    int excludeBitMask = getMultipleBitmask<ExcludeComponents...>();
    // for each component, extract the target vectors of that component in each
    // Archetype (SoA), then get its address (ChunkView)

    std::vector<ChunkView<Components...>> chunks;
    chunks.reserve(m_archetypes.size());

    for (auto &[mask, archetype] : m_archetypes) {
      if ((mask & bitMask) != bitMask)
        continue;
      if ((mask & excludeBitMask) != 0)
        continue;

      ChunkView<Components...> chunk{
          std::tuple<Components *...>{
              archetype.getComponent<Components>().data()...},
          archetype.m_entities, archetype.m_entities.size()};
      chunks.emplace_back(chunk);
    }

    return chunks;
  }
  template <ECSComponent... Components, ECSComponent... ExcludeComponents>
  std::vector<ChunkViewConst<const Components...>>
  queryConst(const exclude_t<ExcludeComponents...> = {}) const
  {
    int bitMask = getMultipleBitmask<Components...>();
    int excludeBitMask = getMultipleBitmask<ExcludeComponents...>();

    std::vector<ChunkViewConst<const Components...>> chunks;
    chunks.reserve(m_archetypes.size());

    for (auto &[mask, archetype] : m_archetypes) {
      if ((mask & bitMask) != bitMask)
        continue;
      if ((mask & excludeBitMask) != 0)
        continue;

      ChunkViewConst<const Components...> chunk{
          std::tuple<const Components *...>{
              std::as_const(archetype).getComponent<Components>().data()...},
          archetype.m_entities, archetype.m_entities.size()};

      chunks.emplace_back(chunk);
    }

    return chunks;
  }
  // ==================================================== //
  // size
  // ==================================================== //

  // return the size of the iterators
  template <ECSComponent... Components, typename... ExcludeComponents>
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
  template <ECSComponent... TargetComponents>
  std::tuple<TargetComponents &...> getComponents(Entity entity)
  {
    static_assert(hasAllBitmask<TargetComponents...>(),
                  "When using getComponent some component didn't exist inside  "
                  "the CompileTimeBitMask");
    const Bitmask b = m_records[static_cast<unsigned>(entity)].bitmask;
    P_ASSERT((b & getMultipleBitmask<TargetComponents...>()) ==
                 getMultipleBitmask<TargetComponents...>(),
             "Entity {} does not have all components requested", entity);
    Archetype &archetype =
        m_archetypes[m_records[static_cast<unsigned>(entity)].bitmask];
    return archetype.extractColumn<TargetComponents...>(
        m_records[static_cast<unsigned>(entity)].column);
  }
  template <ECSComponent... TargetComponents>
  const std::tuple<TargetComponents &...> getComponents(Entity entity) const
  {
    static_assert(hasAllBitmask<TargetComponents...>(),
                  "When using getComponent some component didn't exist inside "
                  "the CompileTimeBitMask");
    const Archetype &archetype =
        m_archetypes.at(m_records[static_cast<unsigned>(entity)].bitmask);
    return archetype.extractColumn<TargetComponents...>(
        m_records[static_cast<unsigned>(entity)].column);
  }
  template <ECSComponent T> T &getComponent(Entity entity)
  {
    Archetype &archetype =
        m_archetypes.at(m_records[static_cast<unsigned>(entity)].bitmask);
    return archetype.fetchComponent<T>(
        m_records[static_cast<unsigned>(entity)].column);
  }
  template <ECSComponent T> const T &getComponent(Entity entity) const
  {
    const Archetype &archetype =
        m_archetypes.at(m_records[static_cast<unsigned>(entity)].bitmask);
    return archetype.fetchComponent<T>(
        m_records[static_cast<unsigned>(entity)].column);
  }

  // ==================================================== //
  // has
  // ==================================================== //

  // Are all components inside the targetBitMask?
  template <ECSComponent... ObjectComponents>
  constexpr bool containsAll(Entity entity) const
  {
    Bitmask targetBitMask = m_records[static_cast<unsigned>(entity)].bitmask;
    int objectBitMask = getMultipleBitmask<ObjectComponents...>();
    return (targetBitMask | objectBitMask) == objectBitMask;
  }
  // Targeted has at least one component that a specific archetype also has
  template <ECSComponent... TargetComponents> bool hasAny(Entity entity) const
  {
    Bitmask specificBitMask = m_records[static_cast<unsigned>(entity)].bitmask;
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
  void remove(Entity entity)
  {
    P_ASSERT(m_records.size() > static_cast<unsigned>(entity),
             "Record couldn't find entity {}", entity);
    // internally: update swapped entity's record, turn removed record invalid
    Bitmask targetBitMask = m_records[static_cast<unsigned>(entity)].bitmask;
    Column targetColumn = m_records[static_cast<unsigned>(entity)].column;
    Archetype &archetype = m_archetypes.at(targetBitMask);

    auto [lastEntityColumn, swappedEntity] = archetype.remove(targetColumn);
    removeEntity(entity);
    m_records[static_cast<unsigned>(swappedEntity)] = {targetBitMask,
                                                       targetColumn};
  }
  template <ECSComponent... ObjectComponents>
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
      Bitmask &bitmask = m_records[target].bitmask;
      Column &column = m_records[target].column;
      Archetype &archetype = m_archetypes.at(bitmask);
      replacedMap.emplace(bitmask,
                          archetype.remove<ObjectComponents...>(column));
      bitmask = Bitmask{-1};
      column = Column{-1};
    }
    // update remaining records columns
    std::size_t updated = 0;
    for (Record &record : m_records) {
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
  static constexpr std::string_view bitmaskWarning =
      "You somehow reached a point where one of the requested components "
      "on your engine doesn't exist in the compile time bitmask. This "
      "should have been prevented with some static assertion on my part, "
      "but I let you read this message in case you want to create more "
      "flexible systems and components. For example: to statically check "
      "only when the system is instantiated.\n\n How to statically check "
      "yourself? just use something like:\n"
      "requires(reg::CompileTimeBitMask::allRegistered<YourComponentsHere."
      "..>())";
  // Bitmask related
  template <ECSComponent... Components>
  static constexpr Bitmask getMultipleBitmask()
  {
    if constexpr (hasAllBitmask<Components...>()) {
      return ComponentManagerT::template multiComponentBitmask<Components...>();
    }
    std::cout << bitmaskWarning << std::endl;
    std::terminate();
    return Bitmask{-1};
  }
  template <ECSComponent Component> static constexpr Bitmask getSingleBitmask()
  {
    if constexpr (hasBitmask<Component>()) {
      return ComponentManagerT::template singleComponentBitmask<Component>();
    }
    std::cout << bitmaskWarning << std::endl;
    std::terminate();
    return Bitmask{-1};
  }
  template <ECSComponent... Component> static constexpr bool hasAllBitmask()
  {
    return ComponentManagerT::template allRegistered<Component...>();
  }
  template <ECSComponent Component> static constexpr bool hasBitmask()
  {
    return ComponentManagerT::template isRegistered<Component>();
  }

  // remove
  void removeEntity(Entity entity)
  {
    m_records[static_cast<unsigned>(entity)].bitmask = Bitmask{-1};
    m_records[static_cast<unsigned>(entity)].column = Column{-1};
    m_availableEntities.push(entity);
  }

  // Record related
  void addRecord(Bitmask bitmask, Column column)
  {
    m_records.emplace_back(bitmask, column);
  }
  void addRecord(Bitmask bitmask, size_t column)
  {
    m_records.emplace_back(bitmask, Column{static_cast<int32_t>(column)});
  }
  void updateRecord(Entity entity, Bitmask bitmask, Column column)
  {
    m_records[static_cast<unsigned>(entity)] = {bitmask, column};
  }
  void updateRecord(Entity entity, Bitmask bitmask, size_t column)
  {
    m_records[static_cast<unsigned>(entity)] = {
        bitmask, Column{static_cast<int32_t>(column)}};
  }
};
} // namespace reg
