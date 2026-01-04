#pragma once

#include "ECS/Registry/Entity.h"
#include "Misc/BasicShape.h"
#include "Physics/Collision/CollisionNaiveSys.h"
#include "Physics/Collision/SweepAndPruneSys.h"
#include <variant>
namespace pain
{

struct ColliderComponent {
  // Offset from transform position
  glm::vec2 m_offset{0.0f, 0.0f};
  std::variant<CircleShape, AABBShape, CapsuleShape> m_shape{AABBShape{}};
  bool m_isTrigger{false}; // Is this a trigger or solid collider? By default,
                           // // all colliders will bounce of each other

  static ColliderComponent createAABB(const glm::vec2 &size = {0.1f, 0.1f},
                                      const glm::vec2 &offset = {0.0f, 0.0f},
                                      bool isTrigger = false)
  {
    return ColliderComponent{.m_offset = offset,
                             .m_shape = AABBShape{size * 0.5f},
                             .m_isTrigger = isTrigger};
  }
  static ColliderComponent createCircle(float radius,
                                        const glm::vec2 &offset = {0.0f, 0.0f},
                                        bool isTrigger = false)
  {
    return ColliderComponent{.m_offset = offset,
                             .m_shape = CircleShape{radius},
                             .m_isTrigger = isTrigger};
  }
  static ColliderComponent createCapsule(float capsuleHeight,
                                         float capsuleSemiCircleRadius,
                                         const glm::vec2 &offset = {0.0f, 0.0f},
                                         bool isTrigger = false)
  {
    return ColliderComponent{
        .m_offset = offset,
        .m_shape = CapsuleShape{capsuleHeight, capsuleSemiCircleRadius},
        .m_isTrigger = isTrigger};
  }
};

// SAP needs an extra id to work with the entity
struct SAPCollider {
  // TODO: capsuple collider case
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
  static SAPCollider createAABB(const glm::vec2 &size = {0.1f, 0.1f},
                                bool isTrigger = false,
                                const glm::vec2 &offset = {0.0f, 0.0f})
  {
    return SAPCollider{.m_offset = offset,
                       .m_shape = AABBShape{size * 0.5f},
                       .m_isTrigger = isTrigger};
  }
  // Create Circle Collider, you NEED TO ADD to SweepAndPruneSys later
  // If you don't want to wait, you can use createAndAddCollider instead
  static SAPCollider createCircle(float radius, bool isTrigger = false,
                                  const glm::vec2 &offset = {0.0f, 0.0f})
  {
    return SAPCollider{.m_offset = offset,
                       .m_shape = CircleShape{radius},
                       .m_isTrigger = isTrigger};
  }

  // ====================================================-===========//
  // Create component and add immediately to Sweep and Prune System
  // ==-=============================================================//

  // Create AABB Collider, then immediately add to SweepAndPruneSys
  static SAPCollider createAndAddAABB(reg::Entity entity,
                                      Transform2dComponent &tc,
                                      const glm::vec2 &size = {0.1f, 0.1f},
                                      bool isTrigger = false,
                                      const glm::vec2 &offset = {0.0f, 0.0f})
  {
    SAPCollider sc{.m_offset = offset,
                   .m_shape = AABBShape{size * 0.5f},
                   .m_isTrigger = isTrigger};

    sc.insertStaticEntity(entity, tc);
    return sc;
  }

  static SAPCollider createAndAddCircle(reg::Entity entity,
                                        Transform2dComponent &tc, float radius,
                                        bool isTrigger = false,
                                        const glm::vec2 &offset = {0.0f, 0.0f})
  {
    SAPCollider sc{.m_offset = offset,
                   .m_shape = CircleShape{radius},
                   .m_isTrigger = isTrigger};

    sc.insertStaticEntity(entity, tc);
    return sc;
  }

protected:
  void insertStaticEntity(reg::Entity entity, Transform2dComponent &tc);
};

} // namespace pain
