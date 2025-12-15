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
private:
  std::map<std::type_index, void *> m_componentMap;
  size_t m_count = 0;

public:
  // ---------------------------------------------------- //
  // create
  // ---------------------------------------------------- //

  // either finds or create a new component
  template <typename C> std::vector<C> &createComponent()
  {
    auto it = m_componentMap.find(std::type_index(typeid(C)));
    if (it != m_componentMap.end()) {
      return *static_cast<std::vector<C> *>(it->second);
    } else {
      auto [newIt, isInserted] = m_componentMap.emplace(
          std::type_index(typeid(C)), new std::vector<C>());
      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New component bitmask added {}", typeid(C).name());
      return *static_cast<std::vector<C> *>(newIt->second);
    }
  }

  template <typename... Components> Column pushComponents(Components &&...comps)
  {
    Column column = Column{-1};
    (...,
     (column = pushComponent<Components>(std::forward<Components>(
          comps)))); // and this should create its columns
    ++m_count;
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

  template <typename... Components> std::int32_t remove(Column column)
  {
    static_assert(sizeof...(Components) != 0,
                  "Not allowed to remove from empty archetypes");
    std::int32_t lastColumn = -1;
    (..., (lastColumn = removeFromComponent<Components>(column)));
    --m_count;
    return lastColumn;
  }

  // This is returning the swapped element's column. You need to use it to
  // update its entity record later
  template <typename C> std::int32_t removeFromComponent(Column index)
  {
    std::vector<C> &v = getComponent<C>();
    std::iter_swap(v.begin() + index, v.end() - 1);
    v.pop_back();
    return v.size();
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
    return componentArray[(unsigned)entityIndex];
  }

  template <typename T> T &fetchComponent(Column entityIndex)
  {
    std::vector<T> &componentArray = getComponent<T>();
    P_ASSERT((unsigned)entityIndex <= componentArray.size(),
             "Entity index {} out of range on array of size {}",
             entityIndex.value, componentArray.size());
    return componentArray[(unsigned)entityIndex];
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
    return *static_cast<const std::vector<C> *>(it->second);
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
    return *static_cast<std::vector<C> *>(it->second);
  }

  std::size_t size() const { return m_count; }
};

} // namespace reg
