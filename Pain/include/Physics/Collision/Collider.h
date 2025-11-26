#pragma once

#include "glm/ext/vector_float2.hpp"
#include <variant>
namespace pain
{

struct CirleShape {
  float radius;
};
struct AABBShape {
  glm::vec2 halfSize;
};
struct CapsuleShape {
  float height;
  float radius;
};
struct PolygonShape {
  // TODO: not in the mood to make this right now
};

struct ColliderComponent {
  glm::vec2 m_offset{0.0f, 0.0f}; // Offset from transform position
  std::variant<CirleShape, AABBShape, CapsuleShape> m_shape;
  bool m_isTrigger = false; // Is this a trigger or solid collider? By default,
                            // all colliders will bounce of each other

  static ColliderComponent
  createCollider(const glm::vec2 &size = {1.0f, 1.0f},
                 const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  {
    return ColliderComponent(size, offset, isTrigger);
  }
  static ColliderComponent
  createCollider(float radius = 1.f, const glm::vec2 &offset = {0.0f, 0.0f},
                 bool isTrigger = false)
  {
    return ColliderComponent(radius, offset, isTrigger);
  }
  static ColliderComponent
  createCollider(float capsuleHeigh = 1.f, float capsuleSemiCircleRadius = 1.f,
                 const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  {
    return ColliderComponent(capsuleHeigh, capsuleSemiCircleRadius, offset,
                             isTrigger);
  }

private:
  ColliderComponent() = default;
  ColliderComponent(const glm::vec2 &size, const glm::vec2 &offset,
                    bool isTrigger)
      : m_offset(offset), m_shape(AABBShape{size}), m_isTrigger(isTrigger) {};
  ColliderComponent(float radius, const glm::vec2 &offset, bool isTrigger)
      : m_offset(offset), m_shape(CirleShape{radius}),
        m_isTrigger(isTrigger) {};
  ColliderComponent(float capsuleHeigh, float capsuleSemiCircleRadius,
                    const glm::vec2 &offset, bool isTrigger)
      : m_offset(offset),
        m_shape(CapsuleShape{capsuleHeigh, capsuleSemiCircleRadius}),
        m_isTrigger(isTrigger) {};
};

} // namespace pain
