#include "Physics/Collision/Collider.h"
#include "Physics/Collision/SweepAndPruneSys.h"

namespace pain
{

// ====================================================-===========//
// Deferred creation (not added to Sweep and Prune)
// ==-=============================================================//

SAPCollider SAPCollider::createAABB(const glm::vec2 &size, bool isTrigger,
                                    const glm::vec2 &offset)
{
  return SAPCollider{.m_offset = offset,
                     .m_shape = AABBShape{size * 0.5f},
                     .m_isTrigger = isTrigger};
}

SAPCollider SAPCollider::createCircle(float radius, bool isTrigger,
                                      const glm::vec2 &offset)
{
  return SAPCollider{.m_offset = offset,
                     .m_shape = CircleShape{radius},
                     .m_isTrigger = isTrigger};
}

// ====================================================-===========//
// Immediate insertion into Sweep and Prune
// ==-=============================================================//

SAPCollider SAPCollider::createStaticAABB(Systems::SweepAndPruneSys &sys,
                                          reg::Entity entity,
                                          Transform2dComponent &tc,
                                          const glm::vec2 &size, bool isTrigger,
                                          const glm::vec2 &offset)
{
  SAPCollider sc{.m_offset = offset,
                 .m_shape = AABBShape{size * 0.5f},
                 .m_isTrigger = isTrigger};

  sys.insertColliderDirectly(entity, tc, sc);
  return sc;
}

SAPCollider SAPCollider::createStaticCircle(Systems::SweepAndPruneSys &sys,
                                            reg::Entity entity,
                                            Transform2dComponent &tc,
                                            float radius, bool isTrigger,
                                            const glm::vec2 &offset)
{
  SAPCollider sc{.m_offset = offset,
                 .m_shape = CircleShape{radius},
                 .m_isTrigger = isTrigger};

  sys.insertColliderDirectly(entity, tc, sc);
  return sc;
}

} // namespace pain
