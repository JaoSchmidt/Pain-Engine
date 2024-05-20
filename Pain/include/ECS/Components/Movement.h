#pragma once

#include "ECS/Components/Rotation.h"
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

struct MovementComponent : TransformComponent {
  float m_translationSpeed = 1.0f;

  MovementComponent() = default;
  MovementComponent(const MovementComponent &) = default;
  MovementComponent(glm::vec3 position, float translationSpeed)
      : TransformComponent(position), m_translationSpeed(translationSpeed)
  {
  }
};

struct MovementSystem {
  static void update(std::unordered_map<Entity, MovementComponent> &map,
                     std::unordered_map<Entity, RotationComponent> &rotMap,
                     DeltaTime dt)
  {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    for (auto pair : map) {
      if (rotMap.find(pair.first) != rotMap.end()) {
        RotationComponent rc = rotMap[pair.first];
        glm::vec3 m_pos = pair.second.m_position;

        float moveAmount = (float)(dt * pair.second.m_translationSpeed * 1.0 +
                                   10.0 * state[SDL_SCANCODE_LSHIFT]);

        if (state[SDL_SCANCODE_W])
          m_pos -=
              glm::cross(rc.m_rotDirection, {0.0f, 0.0f, 1.0f}) * moveAmount;
        if (state[SDL_SCANCODE_S])
          m_pos +=
              glm::cross(rc.m_rotDirection, {0.0f, 0.0f, 1.0f}) * moveAmount;

        if (state[SDL_SCANCODE_A])
          m_pos -= rc.m_rotDirection * moveAmount;
        if (state[SDL_SCANCODE_D])
          m_pos += rc.m_rotDirection * moveAmount;

        if (state[SDL_SCANCODE_Q]) {
          rc.m_rotation += rc.m_rotationSpeed * dt;
          // setRotation(m_rotation);
        }
        if (state[SDL_SCANCODE_E]) {
          rc.m_rotation -= rc.m_rotationSpeed * dt;
          // setRotation(m_rotation);
        }
      }
    }
  }
};

} // namespace pain
