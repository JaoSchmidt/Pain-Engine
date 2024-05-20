#pragma once

#include "pch.h"

#include "CoreFiles/DeltaTime.h"
#include "ECS/Entity.h"

namespace pain
{

struct MovementComponent {
  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  float m_translationSpeed = 1.0f;

  MovementComponent() = default;
  MovementComponent(const MovementComponent &) = default;
  MovementComponent(const glm::vec3 &position, float translationSpeed)
      : m_position(position), m_translationSpeed(translationSpeed)
  {
  }

  operator glm::vec3 &() { return m_position; }
  operator const glm::vec3 &() const { return m_position; }
};

struct MovementSystem {
  static void update(std::unordered_map<Entity, MovementComponent> map,
                     DeltaTime dt)
  {
    for (auto i = map.begin(); i != map.end(); i++) {
    }
  }
};

} // namespace pain
