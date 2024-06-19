#include "ECS/Registry.h"

#include <stdexcept>

namespace pain
{

// template <typename T> T &Registry::get(Entity entity)
// {
//   std::unordered_map<Entity, T> &componentMap = getComponentMap<T>();
//   auto it = componentMap.find(entity);
//   if (it == componentMap.end()) {
//     throw std::runtime_error("Entity does not have the requested component");
//   }
//   return it->second;
// }

// template <typename T> const T &Registry::get(Entity entity, T a) const
// {
//   auto &componentMap = getComponentMap<T>();
//   auto it = componentMap.find(entity);
//   if (it == componentMap.end()) {
//     throw std::runtime_error("Entity does not have the requested component");
//   }
//   return it->second;
// };

// template <typename T> bool Registry::has(Entity entity)
// {
//   const auto &componentMap = getComponentMap<T>();
//   return componentMap.find(entity) != componentMap.end();
// }

// template <typename T> void Registry::remove(Entity entity)
// {
//   auto &componentMap = getComponentMap<T>();
//   componentMap.erase(entity);
// }

void Registry::updateSystems(double dt)
{
  MovementSystem::update(m_movement, m_transforms, dt);
  RotationSystem::update(m_rotation);
}

void Registry::updateSystems(const SDL_Event &e) {}

template <>
std::unordered_map<Entity, TransformComponent> &
Registry::getComponentMap<TransformComponent>()
{
  return m_transforms;
}
template <>
std::unordered_map<Entity, RotationComponent> &
Registry::getComponentMap<RotationComponent>()
{
  return m_rotation;
}
template <>
std::unordered_map<Entity, MovementComponent> &
Registry::getComponentMap<MovementComponent>()
{
  return m_movement;
}

template <> void Registry::remove<MovementComponent>(Entity entity)
{
  auto &componentMap = getComponentMap<MovementComponent>();
  componentMap.erase(entity);
}
template <> void Registry::remove<TransformComponent>(Entity entity)
{
  auto &componentMap = getComponentMap<TransformComponent>();
  componentMap.erase(entity);
}
template <> void Registry::remove<RotationComponent>(Entity entity)
{
  auto &componentMap = getComponentMap<RotationComponent>();
  componentMap.erase(entity);
}

template <> bool Registry::has<MovementComponent>(Entity entity)
{
  const auto &componentMap = getComponentMap<MovementComponent>();
  return componentMap.find(entity) != componentMap.end();
}
template <> bool Registry::has<TransformComponent>(Entity entity)
{
  const auto &componentMap = getComponentMap<TransformComponent>();
  return componentMap.find(entity) != componentMap.end();
}
template <> bool Registry::has<RotationComponent>(Entity entity)
{
  const auto &componentMap = getComponentMap<RotationComponent>();
  return componentMap.find(entity) != componentMap.end();
}

template <> TransformComponent &Registry::get(Entity entity)
{
  std::unordered_map<Entity, TransformComponent> &componentMap =
      getComponentMap<TransformComponent>();
  auto it = componentMap.find(entity);
  if (it == componentMap.end()) {
    throw std::runtime_error("Entity does not have the requested component");
  }
  return it->second;
}

template <> RotationComponent &Registry::get(Entity entity)
{
  std::unordered_map<Entity, RotationComponent> &componentMap =
      getComponentMap<RotationComponent>();
  auto it = componentMap.find(entity);
  if (it == componentMap.end()) {
    throw std::runtime_error("Entity does not have the requested component");
  }
  return it->second;
}

template <> MovementComponent &Registry::get(Entity entity)
{
  std::unordered_map<Entity, MovementComponent> &componentMap =
      getComponentMap<MovementComponent>();
  auto it = componentMap.find(entity);
  if (it == componentMap.end()) {
    throw std::runtime_error("Entity does not have the requested component");
  }
  return it->second;
}

// template <typename T, typename... Args>
// T &Registry::add(Entity entity, Args &&...args)
// {
//   auto &componentMap = getComponentMap<T>();
//   auto [it, inserted] =
//       componentMap.emplace(entity, T(std::forward<Args>(args)...));
//   if (!inserted) {
//     throw std::runtime_error("Component already exists for the entity");
//   }
//   return it->second;
// }

} // namespace pain
