#pragma once

#include "ECS/Components/Movement.h"
#include "ECS/Systems.h"
namespace pain
{

namespace Systems
{
struct NaiveCollisionSystem : public System<ComponentManager> {
  template <typename... Args>
  NaiveCollisionSystem(float cellSize, Args &&...args)
      : System(std::forward<Args>(args)...){};
  NaiveCollisionSystem() = delete;
  void onUpdate(double dt);
  std::vector<std::tuple<const ColliderComponent *, TransformComponent *,
                         MovementComponent *>>
      m_movingObjects;
  std::vector<std::tuple<const ColliderComponent *, TransformComponent *>>
      m_staticObjects;
};

} // namespace Systems
} // namespace pain
