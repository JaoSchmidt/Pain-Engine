#pragma once

#include "CoreFiles/LogWrapper.h"
#include "pch.h"

namespace pain
{

struct TransformComponent {
  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  TransformComponent() = default;
  TransformComponent(const glm::vec3 &position) : m_position(position) {}
  TransformComponent(const glm::vec2 &position)
      : m_position(glm::vec3(position, 0.0f)) {};
  operator glm::vec3 &() { return m_position; }
  operator const glm::vec3 &() const { return m_position; }
};

struct MovementComponent {
  glm::vec3 m_velocity = {0.0f, 0.0f, 0.0f};
  float m_rotationSpeed = 0.0f;
  MovementComponent() = default;
  MovementComponent(const glm::vec3 &velocity) : m_velocity(velocity) {}
  MovementComponent(const glm::vec3 &velocity, float rotationSpeed)
      : m_velocity(velocity), m_rotationSpeed(rotationSpeed)
  {
  }
};

} // namespace pain
