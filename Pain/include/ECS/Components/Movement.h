#pragma once

#include "CoreFiles/LogWrapper.h"
#include "pch.h"

namespace pain
{

struct Transform2dComponent {
  glm::vec2 m_position = {0.0f, 0.0f};
  Transform2dComponent() = default;
  Transform2dComponent(const glm::vec2 &position) : m_position(position) {}
  operator glm::vec2 &() { return m_position; }
  operator const glm::vec2 &() const { return m_position; }
};

struct Movement2dComponent {
  glm::vec2 m_velocity = {0.0f, 0.0f};
  float m_rotationSpeed = 0.0f;
  Movement2dComponent() = default;
  Movement2dComponent(const glm::vec2 &velocity) : m_velocity(velocity) {}
  Movement2dComponent(const glm::vec2 &velocity, float rotationSpeed)
      : m_velocity(velocity), m_rotationSpeed(rotationSpeed) {};
};

} // namespace pain
