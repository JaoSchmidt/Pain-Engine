#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "pch.h"

namespace pain
{

struct RotationComponent {
  using tag = tag::Rotation;
  float m_rotationAngle{0.0f};
  glm::vec3 m_rotation{0.0f, 1.0f, 0.0f};
  static RotationComponent create(const glm::vec3 &rotation)
  {
    return RotationComponent{.m_rotation = rotation};
  }
  static RotationComponent create(const glm::vec3 &rotation,
                                  float rotationAngle)
  {
    return RotationComponent{
        .m_rotationAngle = rotationAngle,
        .m_rotation = rotation,
    };
  }
};

} // namespace pain
