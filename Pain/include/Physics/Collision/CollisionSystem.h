#pragma once

#include "ECS/Components/Movement.h"
#include "ECS/Systems.h"
#include "Physics/Collision/GridManager.h"
namespace pain
{

namespace Systems
{
struct CollisionSystem : public System {
  using System::System;
  void onUpdate(double dt);

  GridManager m_gridManager;
  void solidCollision(Entity entity1, Entity entity2);
};

} // namespace Systems
} // namespace pain
