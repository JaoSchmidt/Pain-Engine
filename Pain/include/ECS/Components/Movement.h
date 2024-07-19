#pragma once

#include "pch.h"

namespace pain
{

struct TransformComponent {
  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  TransformComponent() = default;
  TransformComponent(glm::vec3 position) : m_position(position) {}
  TransformComponent(glm::vec2 position) : m_position(glm::vec3(position, 0.0f))
  {
  }
  operator glm::vec3 &() { return m_position; }
  operator const glm::vec3 &() const { return m_position; }
};

struct MovementComponent {
  glm::vec3 m_velocityDir = {0.0f, 0.0f, 0.0f};
  float m_translationSpeed = 1.0f;
  float m_rotationSpeed = 180.0f;
  MovementComponent() = default;
  MovementComponent(glm::vec3 velocityDir, float translationSpeed,
                    float rotationSpeed)
      : m_velocityDir(velocityDir), m_translationSpeed(translationSpeed),
        m_rotationSpeed(rotationSpeed)
  {
  }
};

} // namespace pain
