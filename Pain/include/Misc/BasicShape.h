#pragma once

#include "Core.h"
#include "glm/ext/vector_float2.hpp"

namespace pain
{

struct CirleShape {
  float radius = 0.1f;
};
struct AABBShape {
  glm::vec2 halfSize = {0.1f, 0.1f}; // half of a size of the rectangle
};
struct QuadShape {
  glm::vec2 size = {0.1f, 0.1f}; // full size of the rectangle ?
};

struct CapsuleShape {
  float height = 0.1f;
  float radius = 0.1f;
};
struct PolygonShape {
  // TODO: not in the mood to make this right now
};

} // namespace pain
