#pragma once

#include "pch.h"

#include "CoreFiles/DeltaTime.h"
#include "ECS/Entity.h"

namespace pain
{

struct RotationComponent {
  glm::vec3 m_rotDirection = {0.0f, 0.0f, 0.0f};
  float m_rotation = 0.0f;
  float m_rotationSpeed = 180.0f;
  RotationComponent() = default;
  RotationComponent(float rotation, float rotationSpeed)
      : m_rotation(rotation), m_rotationSpeed(rotationSpeed)
  {
  }
};

struct RotationSystem {
  // we'll use a constant time for this demonstration
  float dt = 0.1f;

  void update(std::unordered_map<Entity, RotationComponent> map, DeltaTime dt)
  {
    for (auto pair : map) {
      pair.second.m_rotDirection = {cos(glm::radians(pair.second.m_rotation)),
                                    sin(glm::radians(pair.second.m_rotation)),
                                    0};
    }
  }
};

} // namespace pain
