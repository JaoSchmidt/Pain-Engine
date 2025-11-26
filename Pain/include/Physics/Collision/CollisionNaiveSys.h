#pragma once

#include "ECS/Components/Movement.h"
#include "ECS/Systems.h"
#include "Physics/Collision/GridManager.h"
namespace pain
{

namespace Systems
{
struct CollisionSystem : public System<ComponentManager> {
  template <typename... Args>
  CollisionSystem(float cellSize, Args &&...args)
      : System(std::forward<Args>(args)...){};
  CollisionSystem() = delete;
  void onUpdate(double dt);
  std::vector<std::tuple<const ColliderComponent *, TransformComponent *,
                         MovementComponent *>>
      m_movingObjects;
  std::vector<std::tuple<const ColliderComponent *, TransformComponent *>>
      m_staticObjects;
};

} // namespace Systems
} // namespace pain
