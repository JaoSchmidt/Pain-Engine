#include "ECS/Registry.h"
#include "ECS/GameObjWrapper.h"

#include <stdexcept>

namespace pain
{
template <typename T, typename... Args>
T &Registry::add(Entity entity, Args &&...args)
{
  auto &componentMap = getComponentMap<T>();
  auto [it, inserted] =
      componentMap.emplace(entity, T(std::forward<Args>(args)...));
  if (!inserted) {
    throw std::runtime_error("Component already exists for the entity");
  }
  return it->second;
}

template <typename T> T &Registry::get(Entity entity)
{
  auto &componentMap = getComponentMap<T>();
  auto it = componentMap.find(entity);
  if (it == componentMap.end()) {
    throw std::runtime_error("Entity does not have the requested component");
  }
  return it->second;
}

template <typename T> const T &Registry::get(Entity entity) const
{
  auto &componentMap = getComponentMap<T>();
  auto it = componentMap.find(entity);
  if (it == componentMap.end()) {
    throw std::runtime_error("Entity does not have the requested component");
  }
  return it->second;
};

template <typename T> bool Registry::has(Entity entity)
{
  const auto &componentMap = getComponentMap<T>();
  return componentMap.find(entity) != componentMap.end();
}

template <typename T> void Registry::remove(Entity entity)
{
  auto &componentMap = getComponentMap<T>();
  componentMap.erase(entity);
}

} // namespace pain
