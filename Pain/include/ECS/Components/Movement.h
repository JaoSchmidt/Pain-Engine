#pragma once

#include "pch.h"

#include "CoreFiles/DeltaTime.h"
#include "ECS/Entity.h"

namespace pain
{

struct TransformComponent {
  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  TransformComponent() = default;
  TransformComponent(const TransformComponent &) = default;
  TransformComponent(glm::vec3 position) : m_position(position) {}
  operator glm::vec3 &() { return m_position; }
  operator const glm::vec3 &() const { return m_position; }
};
// clang-format off
struct MovementComponent {
  glm::vec3 m_velocityDir = {0.0f, 0.0f, 0.0f};
  float m_translationSpeed = 1.0f;
  float m_rotationSpeed = 180.0f;
  MovementComponent() = default;
  MovementComponent(const MovementComponent &) = default;
  MovementComponent(float translationSpeed) : m_translationSpeed(translationSpeed){}
};

struct RotationComponent {
  glm::vec3 m_rotation = {0.0f, 0.0f, 0.0f};
  float m_rotationAngle = 0.0f;
  RotationComponent() = default;
  RotationComponent(glm::vec3 rotation, float rotationAngle, float rotationSpeed)
      : m_rotation(rotation), m_rotationAngle(rotationAngle){}
}; // clang-format on

struct MovementSystem {
  static void update(std::unordered_map<Entity, MovementComponent> &movMap,
                     std::unordered_map<Entity, TransformComponent> &traMap,
                     DeltaTime dt)
  {
    for (auto &pair : movMap) {
      const MovementComponent mc = pair.second;
      TransformComponent tc = traMap[pair.first];
      const float moveAmount = (float)(dt * mc.m_translationSpeed);
      tc.m_position = mc.m_velocityDir * moveAmount;
    }
  }
};

struct RotationSystem {
  static void update(std::unordered_map<Entity, RotationComponent> map)
  {
    for (auto &pair : map) {
      pair.second.m_rotation = {cos(glm::radians(pair.second.m_rotationAngle)),
                                sin(glm::radians(pair.second.m_rotationAngle)),
                                0};
    }
  }
};

} // namespace pain
