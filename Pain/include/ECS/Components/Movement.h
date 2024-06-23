#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "pch.h"

#include "CoreFiles/DeltaTime.h"
#include "ECS/Entity.h"
#include <iostream>

namespace pain
{

struct TransformComponent {
  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  TransformComponent() = default;
  TransformComponent(glm::vec3 position) : m_position(position) {}
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

struct RotationComponent {
  glm::vec3 m_rotation = {0.0f, 1.0f, 0.0f};
  float m_rotationAngle = 0.0f;
  RotationComponent() = default;
  RotationComponent(glm::vec3 rotation, float rotationAngle)
      : m_rotation(rotation), m_rotationAngle(rotationAngle)
  {
  }
};

struct MovementSystem {
  static void
  update(const std::unordered_map<Entity, MovementComponent> &movMap,
         std::unordered_map<Entity, TransformComponent> &traMap, double dt)
  {
    for (auto &pair : movMap) {
      const MovementComponent &mc = pair.second;
      TransformComponent &tc = traMap[pair.first];
      const float moveAmount = (float)(dt * mc.m_translationSpeed);
      tc.m_position = tc.m_position + mc.m_velocityDir * moveAmount;
    }
  }
};

struct RotationSystem {
  static void update(std::unordered_map<Entity, RotationComponent> &map)
  {
    for (auto &pair : map) {
      RotationComponent &rc = pair.second;
      rc.m_rotation = {cos(glm::radians(rc.m_rotationAngle)),
                       sin(glm::radians(rc.m_rotationAngle)), 0};
    }
  }
};

} // namespace pain
