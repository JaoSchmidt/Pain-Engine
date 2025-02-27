#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"
#include <memory>
#include <type_traits>
#include <typeindex>

namespace pain
{

struct Deleter {
  void operator()(void *data) const noexcept { std::free(data); }
};

// ---------------------------------------------------- //
// Container definition
// ---------------------------------------------------- //
class Archetype
{
private:
  std::map<std::type_index, std::unique_ptr<void, Deleter>> m_componentMap;

public:
  std::vector<Entity> m_entities;

  // ---------------------------------------------------- //
  // has
  // ---------------------------------------------------- //

  // Check if archetype has an entity
  bool has(Entity entity) const
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end())
      return false;
    return true;
  }

  // ---------------------------------------------------- //
  // add
  // ---------------------------------------------------- //

  // directly add entity to archetype, should be used once per entity
  void addEntity(Entity entity) { m_entities.push_back(entity); }

  // TODO: Allow use of reserve() function and multiple emplaces

  // directly add the component to the archetype, should be used N time per
  // entity, with N being the entity's number of components
  template <typename C, typename... Args> void pushComponent(Args &&...args)
  {
    std::vector<C> &componentArray = createComponent<C>();
    componentArray.emplace_back(std::forward<Args>(args)...);
  }

  // Remove an entity from the archetype
  template <typename... Components> bool remove(Entity entity)
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end())
      return false;

    // remove entity
    int index = it - m_entities.begin();
    m_entities.erase(m_entities.begin() + index);

    // remove components
    (removeFromComponent<Components>(index), ...);
    return true;
  }

  template <typename C> void removeFromComponent(int index)
  {
    std::vector<C> &v = getComponent<C>();
    v.erase(v.begin() + index);
  }

  inline std::vector<Entity>::const_iterator begin() const
  {
    return m_entities.begin();
  }
  inline std::vector<Entity>::const_iterator end() const
  {
    return m_entities.end();
  }

  // Extract a column of an archetype. I.e. the components of an entity
  template <typename... Components>
  std::tuple<const Components &...> extractColumn(Entity entity) const
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return std::tie<const Components &...>(
        fetchComponent<Components>(index)...);
  };
  template <typename... Components>
  std::tuple<Components &...> extractColumn(Entity entity)
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return std::tie<Components &...>(fetchComponent<Components>(index)...);
  };

  // Same as extractColumn but with extract a single component of the entity
  template <typename T> T &extractComponent(Entity entity)
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return fetchComponent<T>(index);
  }
  template <typename T> const T &extractComponent(Entity entity) const
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return fetchComponent<T>(index);
  }

  template <typename T> const T &fetchComponent(int entityIndex) const
  {
    const std::vector<T> &componentArray = getComponent<T>();
    P_ASSERT((unsigned)entityIndex <= componentArray.size(),
             "Entity index {} out of range on array of size {}", entityIndex,
             componentArray.size());
    return componentArray[entityIndex];
  }

  template <typename T> T &fetchComponent(int entityIndex)
  {
    std::vector<T> &componentArray = getComponent<T>();
    P_ASSERT((unsigned)entityIndex <= componentArray.size(),
             "Entity index {} out of range on array of size {}", entityIndex,
             componentArray.size());
    return componentArray[entityIndex];
  }

  template <typename C> const std::vector<C> &getComponent() const
  {
    return *static_cast<const std::vector<C> *>(
        m_componentMap.at(std::type_index(typeid(C))).get());
  }

  template <typename C> std::vector<C> &getComponent()
  {
    return *static_cast<std::vector<C> *>(
        m_componentMap.at(std::type_index(typeid(C))).get());
  }
  template <typename C> std::vector<C> &createComponent()
  {
    auto it = m_componentMap.find(std::type_index(typeid(C)));
    if (it != m_componentMap.end()) {
      return *static_cast<std::vector<C> *>(it->second.get());
    } else {
      auto [newIt, isInserted] = m_componentMap.emplace(
          std::type_index(typeid(C)),
          std::unique_ptr<void, Deleter>(new std::vector<C>(), Deleter()));
      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New component bitmask added {}", typeid(C).name());
      return *static_cast<std::vector<C> *>(newIt->second.get());
    }
  }
};

} // namespace pain
