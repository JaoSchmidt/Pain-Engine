#pragma once

#include "glm/ext/vector_float2.hpp"
namespace pain
{

struct ColliderComponent {
  glm::vec2 m_offset{0.0f, 0.0f}; // Offset from transform position
  glm::vec2 m_size{1.0f, 1.0f};   // Collider dimensions
  bool m_isTrigger = false; // Is this a trigger or solid collider? By default,
                            // all colliders will bounce of each other

  static ColliderComponent
  createCollider(const glm::vec2 &size = {1.0f, 1.0f},
                 const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  {
    return ColliderComponent(size, offset, isTrigger);
  }

private:
  ColliderComponent() = default;
  ColliderComponent(const glm::vec2 &size, const glm::vec2 &offset,
                    bool isTrigger)
      : m_offset(offset), m_size(size), m_isTrigger(isTrigger) {};
};

} // namespace pain
