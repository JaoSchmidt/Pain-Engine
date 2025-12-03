#pragma once

#include "ECS/Systems.h"
#include "Physics/Collision/Collider.h"
namespace pain
{

namespace Systems
{

struct SweepAndPruneSys : public System<ComponentManager> {
  std::vector<SAPCollider> m_colliders;
};

} // namespace Systems
} // namespace pain
