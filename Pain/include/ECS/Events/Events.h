#pragma once

#include "Core.h"
#include "ECS/Registry/Entity.h"
#include "glm/ext/vector_float2.hpp"

namespace pain
{

struct CollisionEvent {
  reg::Entity a;
  reg::Entity b;
  glm::vec2 normal;
  float penetration;
  bool isTrigger;
};

} // namespace pain
