#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"
#include <tuple>
#include <typeindex>

namespace pain
{

class Archetype
{
  friend class ArcheRegistry;
  Archetype(int bitmask)
  {
    int componentID = 0;
    while (bitmask) {
      if (bitmask & 1) {
        m_components[componentID] = std::vector<void *>();
      }
      bitmask >>= 1;
      ++componentID;
    }
  }
  void addEntity(Entity entity) { m_entities.push_back(entity); }

  template <typename C, typename... Args> void pushComponent(Args &&...args)
  {
    auto &componentArray = m_components[C::componentID];
    componentArray.emplace(C(std::forward<Args>(args)...));
  }

  // Remove an entity from the archetype
  bool remove(Entity entity)
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end())
      return false;

    // remove components
    int index = it - m_entities.begin();
    m_entities.erase(m_entities.begin() + index);
    for (auto &component : m_components) {
      auto &componentVector = component.second;
      componentVector.erase(componentVector.begin() + index);
    }
    return true;
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
  std::tuple<Components *...> extractColumn(Entity entity, int bitMask) const
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return std::make_tuple(fetchComponent<Components>(index, bitMask)...);
  };

  template <typename C> std::vector<void *> &componentVector()
  {
    return m_components.at(C::ComponentID);
  }

  template <typename Component>
  Component *fetchComponent(int entityIndex, int bitMask) const
  {
    auto it = m_components.find(bitMask);
    P_ASSERT(it == m_components.end(), "Component type {} not found",
             entityIndex);

    auto &componentVector = it->second;
    P_ASSERT(entityIndex >= componentVector.size(),
             "Entity index {} out of range", entityIndex);

    return static_cast<Component *>(componentVector[entityIndex]);
  }

  std::vector<Entity> m_entities;
  // "SoA", more like map of vectors
  std::map<int, std::vector<void *>> m_components;
};

} // namespace pain
