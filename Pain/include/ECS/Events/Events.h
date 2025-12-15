#pragma once

#include "Core.h"

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
