#pragma once

#include "ECS/Registry/Entity.h"
#include "Misc/BasicShape.h"
#include <variant>
namespace pain
{

struct ColliderComponent {
  glm::vec2 m_offset{0.0f, 0.0f}; // Offset from transform position
  std::variant<CirleShape, AABBShape, CapsuleShape> m_shape = AABBShape();
  bool m_isTrigger = false; // Is this a trigger or solid collider? By default,
                            // all colliders will bounce of each other

  static ColliderComponent
  createCollider(const glm::vec2 &size = {0.1f, 0.1f},
                 const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  {
    return ColliderComponent(size * 0.5f, offset, isTrigger);
  }
  static ColliderComponent
  createCollider(float radius, const glm::vec2 &offset = {0.0f, 0.0f},
                 bool isTrigger = false)
  {
    return ColliderComponent(radius, offset, isTrigger);
  }
  static ColliderComponent
  createCollider(float capsuleHeigh, float capsuleSemiCircleRadius,
                 const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  {
    return ColliderComponent(capsuleHeigh, capsuleSemiCircleRadius, offset,
                             isTrigger);
  }

protected:
  ColliderComponent() = default;
  ColliderComponent(const glm::vec2 &halfSize, const glm::vec2 &offset,
                    bool isTrigger)
      : m_offset(offset), m_shape(AABBShape{halfSize}),
        m_isTrigger(isTrigger) {};
  ColliderComponent(float radius, const glm::vec2 &offset, bool isTrigger)
      : m_offset(offset), m_shape(CirleShape{radius}),
        m_isTrigger(isTrigger) {};
  ColliderComponent(float capsuleHeigh, float capsuleSemiCircleRadius,
                    const glm::vec2 &offset, bool isTrigger)
      : m_offset(offset),
        m_shape(CapsuleShape{capsuleHeigh, capsuleSemiCircleRadius}),
        m_isTrigger(isTrigger) {};
};

struct SAPCollider : public ColliderComponent {
  reg::Entity id;
};

} // namespace pain
