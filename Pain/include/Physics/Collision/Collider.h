#pragma once

#include "ECS/Registry/Entity.h"
#include "Misc/BasicShape.h"
#include "Physics/Collision/CollisionNaiveSys.h"
#include "Physics/Collision/SweepAndPruneSys.h"
#include <variant>
namespace pain
{

struct ColliderComponent {
  glm::vec2 m_offset{0.0f, 0.0f}; // Offset from transform position
  std::variant<CircleShape, AABBShape, CapsuleShape> m_shape = AABBShape();
  bool m_isTrigger = false; // Is this a trigger or solid collider? By default,
                            // all colliders will bounce of each other
  static ColliderComponent
  createCollider(const glm::vec2 &size = {0.1f, 0.1f},
                 const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  {
    return ColliderComponent(AABBShape{size * 0.5f}, offset, isTrigger);
  }

  static ColliderComponent
  createCollider(float radius, const glm::vec2 &offset = {0.0f, 0.0f},
                 bool isTrigger = false)
  {
    return ColliderComponent(CircleShape{radius}, offset, isTrigger);
  }

  static ColliderComponent
  createCollider(float capsuleHeight, float capsuleSemiCircleRadius,
                 const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  {
    return ColliderComponent(
        CapsuleShape{capsuleHeight, capsuleSemiCircleRadius}, offset,
        isTrigger);
  }

protected:
  ColliderComponent() = default;
  ColliderComponent(
      const std::variant<CircleShape, AABBShape, CapsuleShape> &shape,
      const glm::vec2 &offset, bool isTrigger)
      : m_offset(offset), m_shape(shape), m_isTrigger(isTrigger) {};
};

// SAP needs an extra id to work with the entity
struct SAPCollider {
  glm::vec2 m_offset{0.0f, 0.0f}; // Offset from transform position
  std::variant<CircleShape, AABBShape, CapsuleShape> m_shape = AABBShape();
  bool m_isTrigger = false; // Is this a trigger or solid collider? By default,
                            // all colliders will bounce of each other
  int m_index = -1;

  // ====================================================-===========//
  // Create this component, need to add them to Sweep and Prune later
  // ==-=============================================================//

  // Create AABB Collider, you NEED TO ADD to SweepAndPruneSys later
  // If you don't want to wait, you can use createAndAddCollider instead
  static SAPCollider createCollider(const glm::vec2 &size = {0.1f, 0.1f},
                                    bool isTrigger = false,
                                    const glm::vec2 &offset = {0.0f, 0.0f})
  {
    return SAPCollider(AABBShape{size * 0.5f}, offset, isTrigger);
  }

  // Create Circle Collider, you NEED TO ADD to SweepAndPruneSys later
  // If you don't want to wait, you can use createAndAddCollider instead
  static SAPCollider createCollider(float radius, bool isTrigger = false,
                                    const glm::vec2 &offset = {0.0f, 0.0f})
  {
    return SAPCollider(CircleShape{radius}, offset, isTrigger);
  }

  // Create Capsule Collider, need to add to SweepAndPruneSys later
  // static SAPCollider createCollider(reg::Entity entity, float capsuleHeight,
  //                                   float capsuleSemiCircleRadius,
  //                                   const glm::vec2 &offset = {0.0f, 0.0f},
  //                                   bool isTrigger = false)
  // {
  //   return SAPCollider(entity,
  //                      CapsuleShape{capsuleHeight, capsuleSemiCircleRadius},
  //                      offset, isTrigger);
  // }

  // ====================================================-===========//
  // Create component and add immediately to Sweep and Prune System
  // ==-=============================================================//

  // Create AABB Collider, then immediately add to SweepAndPruneSys
  static SAPCollider
  createAndAddCollider(reg::Entity entity, Transform2dComponent tc,
                       const glm::vec2 &size = {0.1f, 0.1f},
                       bool isTrigger = false,
                       const glm::vec2 &offset = {0.0f, 0.0f})
  {
    SAPCollider sc(AABBShape{size * 0.5f}, offset, isTrigger);
    sc.insertStaticEntity(entity, tc);
    return sc;
  }
  // Create Circle Collider, then immediately add to SweepAndPruneSys
  static SAPCollider
  createAndAddCollider(reg::Entity entity, Transform2dComponent tc,
                       float radius, bool isTrigger = false,
                       const glm::vec2 &offset = {0.0f, 0.0f})
  {

    SAPCollider sc(CircleShape{radius}, offset, isTrigger);
    sc.insertStaticEntity(entity, tc);
    return sc;
  }

  // Create AABB Collider, then immediately add to SweepAndPruneSys
  // static SAPCollider createAndAddCollider(
  //     reg::Entity entity, float capsuleHeight, float capsuleSemiCircleRadius,
  //     const glm::vec2 &offset = {0.0f, 0.0f}, bool isTrigger = false)
  // {
  //   return SAPCollider(entity,
  //                      CapsuleShape{capsuleHeight, capsuleSemiCircleRadius},
  //                      offset, isTrigger);
  // }

protected:
  void insertStaticEntity(reg::Entity entity, Transform2dComponent &tc);
  SAPCollider(const std::variant<CircleShape, AABBShape, CapsuleShape> &shape,
              const glm::vec2 &offset, bool isTrigger)
      : m_offset(offset), m_shape(shape), m_isTrigger(isTrigger) {};
};

} // namespace pain
