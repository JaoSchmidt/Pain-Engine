#pragma once

#include "pch.h"

#include "ECS/Registry/Macros.h"

namespace pain
{

struct RotationComponent {
  static constexpr int componentID = bitMaskUnit(5);
  glm::vec3 m_rotation = {0.0f, 1.0f, 0.0f};
  float m_rotationAngle = 0.0f;
  RotationComponent() = default;
  RotationComponent(glm::vec3 rotation, float rotationAngle)
      : m_rotation(rotation), m_rotationAngle(rotationAngle)
  {
  }
};

} // namespace pain
