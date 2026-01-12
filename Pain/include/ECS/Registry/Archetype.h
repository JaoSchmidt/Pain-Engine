#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Entity.h"
#include <exception>
#include <typeindex>

namespace reg
{

// ---------------------------------------------------- //
// Container definition
// ---------------------------------------------------- //
class Archetype
{
public:
  using Deleter = void (*)(void *);
  using ErasedVector = std::unique_ptr<void, Deleter>;
  std::vector<std::function<Column(Column)>> m_removers;
  std::vector<reg::Entity> m_entities;

private:
  std::map<std::type_index, ErasedVector> m_componentMap;

public:
  // ---------------------------------------------------- //
  // create
  // ---------------------------------------------------- //

  // either finds or create a new component
  template <typename C> std::vector<C> &createComponent()
  {
    auto it = m_componentMap.find(std::type_index(typeid(C)));
    if (it != m_componentMap.end()) {
      return *static_cast<std::vector<C> *>(it->second.get());
    } else {

      auto deleter = [](void *vector) {
        delete static_cast<std::vector<C> *>(vector);
      };
      m_removers.push_back([this](reg::Column column) {
        return removeFromComponent<C>(column);
      });
      auto [newIt, isInserted] =
          m_componentMap.emplace(std::type_index(typeid(C)),
                                 ErasedVector{new std::vector<C>(), deleter});
      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New component bitmask added {}", typeid(C).name());

      // store the deleter to use inside the destructor
      return *static_cast<std::vector<C> *>(newIt->second.get());
    }
  }
  template <typename... Components> Column pushComponents(Components &&...comps)
  {
    Column column = Column{-1};
    (...,
     (column = pushComponent<Components>(std::forward<Components>(
          comps)))); // and this should create its columns
    return column;
  }
  template <typename... Components>
  Column pushComponents(reg::Entity entity, Components &&...comps)
  {
    Column column = Column{-1};
    (...,
     (column = pushComponent<Components>(std::forward<Components>(
          comps)))); // and this should create its columns
    m_entities.emplace_back(entity);
    return column;
  }

  // directly add the component to the archetype, should be used N time per
  // entity, with N being the entity's number of components
  template <typename C, typename... Args> Column pushComponent(Args &&...args)
  {
    std::vector<C> &componentArray = createComponent<C>();
    Column index = Column(static_cast<int>(componentArray.size()));
    componentArray.emplace_back(std::forward<Args>(args)...);
    return index;
  }

  // ---------------------------------------------------- //
  // remove
  // ---------------------------------------------------- //

  std::pair<Column, Entity> remove(Column column)
  {
    Column lastColumn = Column{-1};
    Entity swappedEntity = m_entities[m_entities.size() - 1];

    for (std::function<Column(Column)> &remover : m_removers) {
      lastColumn = remover(column);
    }
    std::iter_swap(m_entities.begin() + column, m_entities.end() - 1);
    m_entities.pop_back();
    return std::make_pair(lastColumn, swappedEntity);
  }

  template <typename... Components>
  std::pair<Column, Entity> remove(Column column)
  {
    static_assert(sizeof...(Components) != 0,
                  "Not allowed to remove from empty archetypes");
    Column lastColumn = Column{-1};
    Entity swappedEntity = m_entities[m_entities.size() - 1];
    (..., (lastColumn = removeFromComponent<Components>(column)));
    std::iter_swap(m_entities.begin() + column, m_entities.end() - 1);
    m_entities.pop_back();
    return std::make_pair(lastColumn, swappedEntity);
  }

  // This is returning the swapped element's column. You need to use it to
  // update its entity record later
  template <typename C> Column removeFromComponent(Column index)
  {
    std::vector<C> &v = getComponent<C>();
    std::iter_swap(v.begin() + index, v.end() - 1);
    v.pop_back();
    return Column{static_cast<int>(v.size())};
  }

  // ---------------------------------------------------- //
  // get
  // ---------------------------------------------------- //
  // Extract a column of an archetype. I.e. the components of an entity
  template <typename... Components>
  std::tuple<const Components &...> extractColumn(Column column) const
  {
    return std::tie<const Components &...>(
        fetchComponent<Components>(column)...);
  };
  template <typename... Components>
  std::tuple<Components &...> extractColumn(Column column)
  {
    return std::tie<Components &...>(fetchComponent<Components>(column)...);
  };

  template <typename T> const T &fetchComponent(Column entityIndex) const
  {
    const std::vector<T> &componentArray = getComponent<T>();
    P_ASSERT((unsigned)entityIndex <= componentArray.size(),
             "Entity index {} out of range on array of size {}",
             entityIndex.value, componentArray.size());
    return componentArray[static_cast<unsigned>(entityIndex)];
  }

  template <typename T> T &fetchComponent(Column entityIndex)
  {
    std::vector<T> &componentArray = getComponent<T>();
    P_ASSERT((unsigned)entityIndex <= componentArray.size(),
             "Entity index {} out of range on array of size {}",
             entityIndex.value, componentArray.size());
    return componentArray[static_cast<unsigned>(entityIndex)];
  }

  template <typename C> const std::vector<C> &getComponent() const
  {
    auto it = m_componentMap.find(std::type_index(typeid(C)));
    if (it == m_componentMap.end()) {
      PLOG_E("Cannot find component vector inside Archetype");
      PLOG_E("You are probably trying to call getComponent but the component "
             "you want doesn't exist in the object.");
      PLOG_E("Missing component type: {}", typeid(C).name());
      std::terminate();
    }
    return *static_cast<const std::vector<C> *>(it->second.get());
  }

  template <typename C> std::vector<C> &getComponent()
  {
    auto it = m_componentMap.find(std::type_index(typeid(C)));
    if (it == m_componentMap.end()) {
      PLOG_E("Cannot find component vector inside Archetype");
      PLOG_E("You are probably trying to call getComponent but the component "
             "you want doesn't exist in the object.");
      PLOG_E("Missing component type: {}", typeid(C).name());
      std::terminate();
    }
    return *static_cast<std::vector<C> *>(it->second.get());
  }
  Column lastColumn() const
  {
    return Column{static_cast<int>(m_entities.size())};
  }
  reg::Entity getEntity(Column column) const
  {
    return m_entities[static_cast<unsigned>(column)];
  }
};

} // namespace reg
