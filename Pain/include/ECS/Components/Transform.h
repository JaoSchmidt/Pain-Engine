#pragma once
#include "pch.h"

namespace pain
{

struct TransformComponent {
  glm::mat4 Transform{1.0f};

  TransformComponent() = default;
  TransformComponent(const TransformComponent &) = default;
  TransformComponent(const glm::mat4 &transform) : Transform(transform) {}

  operator glm::mat4 &() { return Transform; }
  operator const glm::mat4 &() const { return Transform; }
};
// struct TransformSystem {
//   void update(Scene scene)
//   {
// for (int e = 1; e <= max_entity; e++) {
//   // and each entity with a transform compnent will update
//   // their position with respect to their velocity
//   if (reg.transforms.contains(e)) {
//     reg.transforms[e].pos_x += reg.transforms[e].vel_x * dt;
//     reg.transforms[e].pos_y += reg.transforms[e].vel_y * dt;
//   }
// }
//   }
// };

} // namespace pain
