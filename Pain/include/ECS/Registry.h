#pragma once

#include "pch.h"

#include "Core.h"

#include "ECS/Entity.h"
#include <typeindex>

namespace pain
{

class Registry
{
  friend class Scene;
  Registry() = default;
  // ---------------------------------------------------- //
  // Entities
  // ---------------------------------------------------- //
  template <typename T, typename... Args> T &add(Entity entity, Args &&...args)
  {
    std::unordered_map<Entity, T> &componentMap = getComponentMap<T>();
    auto [it, inserted] =
        componentMap.emplace(entity, T(std::forward<Args>(args)...));
    if (!inserted) {
      throw std::runtime_error("Component already exists for the entity");
    }
    return it->second;
  }

  template <typename T> T &get(Entity entity)
  {
    std::unordered_map<Entity, T> &componentMap = getComponentMap<T>();
    auto it = componentMap.find(entity);
    if (it == componentMap.end()) {
      throw std::runtime_error("Entity does not have the requested component");
    }
    return it->second;
  }

  template <typename T> void remove(Entity entity)
  {
    std::unordered_map<Entity, T> &componentMap = getComponentMap<T>();
    componentMap.erase(entity);
  }

  template <typename T> bool has(Entity entity)
  {
    const auto &componentMap = getComponentMap<T>();
    return componentMap.find(entity) != componentMap.end();
  }

  // ---------------------------------------------------- //
  // Maps
  // ---------------------------------------------------- //
  template <typename T> std::unordered_map<Entity, T> &getComponentMap()
  {
    std::type_index typeIndex = std::type_index(typeid(T));
    return *static_cast<std::unordered_map<Entity, T> *>(
        componentMaps.at(typeIndex));
  }
  template <typename T>
  const std::unordered_map<Entity, T> &getComponentMap() const
  {
    std::type_index typeIndex = std::type_index(typeid(T));
    return *static_cast<std::unordered_map<Entity, T> *>(
        componentMaps.at(typeIndex));
  }
  template <typename T> void addComponentMap()
  {
    std::type_index typeIndex = std::type_index(typeid(T));
    if (componentMaps.find(typeIndex) == componentMaps.end()) {
      componentMaps[typeIndex] = new std::unordered_map<Entity, T>();
    }
  }
  std::unordered_map<std::type_index, void *> componentMaps = {};
};

} // namespace pain
