#include "Physics/Collision/ColReaction.h"

namespace pain
{

void basicImpulseStatic(glm::vec2 &vel1, const glm::vec2 normal)
{
  // Ensure the normal is normalized
  const glm::vec2 relVel = -vel1;

  // If the dynamic object is already moving away, no response
  const float relNormVel = glm::dot(relVel, normal);
  if (relNormVel > 0.f)
    return;

  // Static object = infinite mass → only object 1 reacts
  // Restitution: 1.0 means perfectly elastic
  const float restitution = 1.0f;

  // Impulse scalar (since invMass2 = 0)
  const float j = -(1.f + restitution) * relNormVel;

  // Apply impulse to velocity
  const glm::vec2 impulse = j * normal;
  vel1 += impulse;
}

void basicImpulse(glm::vec2 &vel1,        //
                  glm::vec2 &vel2,        //
                  const glm::vec2 normal, //
                  float mass1, float mass2)
{
  // Relative velocity
  const glm::vec2 relVel = vel2 - vel1;

  // Check if they are already moving apart along the collision normal
  const float relNormVel = glm::dot(relVel, normal);
  if (relNormVel > 0.f)
    return; // no reaction needed

  // Elastic collision impulse
  const float invMass1 = 1.f / mass1;
  const float invMass2 = 1.f / mass2;

  const float restitution = 1.0f; // NOTE: restitution ∈ [0,1]

  float j = -(1.f + restitution) * relNormVel;
  j /= (1.f / mass1 + 1.f / mass2);

  const glm::vec2 impulse = j * normal;
  vel1 -= impulse * invMass1;
  vel2 += impulse * invMass2;
}
// ==========================================================
// AABB = Axis Aligned Bounding Box
// ----------------------------------------------------------

void ColReaction::solidCollisionAABB(const glm::vec2 &pos1,      //
                                     glm::vec2 &vel1,            //
                                     const glm::vec2 &halfSize1, //
                                     const glm::vec2 &pos2,      //
                                     glm::vec2 &vel2,            //
                                     const glm::vec2 &halfSize2, //
                                     float mass1, float mass2)
{
  // Relative position
  const glm::vec2 diff = pos2 - pos1;

  // Overlaps on each axis
  const float overlapX = (halfSize1.x + halfSize2.x) - std::fabs(diff.x);
  const float overlapY = (halfSize1.y + halfSize2.y) - std::fabs(diff.y);

  // Determine collision normal = axis of minimum penetration
  glm::vec2 normal(0.f);

  if (overlapX < overlapY) {
    normal.x = (diff.x < 0.f) ? -1.f : 1.f;
  } else {
    normal.y = (diff.y < 0.f) ? -1.f : 1.f;
  }
  basicImpulse(vel1, vel2, normal, mass1, mass2);
}

void ColReaction::solidCollisionAABBxStaticAABB(glm::vec2 &center1,
                                                glm::vec2 &vel1,
                                                const glm::vec2 &center2)
{
  const glm::vec2 normal = glm::normalize(center2 - center1);
  const float v1n = glm::dot(vel1, normal);
  if (v1n > 0.0f) // already moving away
    return;

  vel1 -= 2.0f * v1n * normal;
}

// ==========================================================
// OBB = Oriented Bounding Box
// ----------------------------------------------------------

void ColReaction::solidCollisionOBB(glm::vec2 &vel1,                  //
                                    glm::vec2 &vel2,                  //
                                    const glm::vec2 &collisionNormal, //
                                    float mass1, float mass2)
{
  basicImpulse(vel1, vel2, collisionNormal, mass1, mass2);
}
void solidCollisionStaticOBBxOBB(glm::vec2 &vel1,
                                 const glm::vec2 &collisionNormal)
{
  basicImpulseStatic(vel1, collisionNormal);
}

// ==========================================================
// CIRCLE = a circle, duh
// ----------------------------------------------------------

void ColReaction::solidCollisionCIRCLE(const glm::vec2 &center1, //
                                       glm::vec2 &vel1,          //
                                       const glm::vec2 &center2, //
                                       glm::vec2 &vel2,          //
                                       float mass1, float mass2)
{
  const glm::vec2 diff = center2 - center1;
  const glm::vec2 normal = glm::normalize(diff);
  basicImpulse(vel1, vel2, normal, mass1, mass2);
}

// Solid collision between a static circle and a circle shapes
void solidCollisionStaticCIRCLExCIRCLE(const glm::vec2 &center1, //
                                       glm::vec2 &vel1,          //
                                       const glm::vec2 &center2)
{
  const glm::vec2 diff = center2 - center1;
  const glm::vec2 normal = glm::normalize(diff);
  basicImpulseStatic(vel1, normal);
}

// ==========================================================
// Mixed stuff, but only squared
// ----------------------------------------------------------

// Solid collision between OBB and AABB shapes
void ColReaction::solidCollisionAABBxOBB(glm::vec2 &vel1,                  //
                                         glm::vec2 &vel2,                  //
                                         const glm::vec2 &collisionNormal, //
                                         float mass1, float mass2)
{
  basicImpulse(vel1, vel2, collisionNormal, mass1, mass2);
}
// Solid collision between OBB and AABB shapes
void solidCollisionStaticAABBxOBB(glm::vec2 &vel2, //
                                  const glm::vec2 &collisionnormal)
{
  basicImpulseStatic(vel2, collisionnormal);
}

// Solid collision between OBB and AABB shapes
void solidCollisionAABBxStaticOBB(glm::vec2 &vel1, //
                                  const glm::vec2 &collisionNormal)
{
  basicImpulseStatic(vel1, collisionNormal);
}

// ==========================================================
// Mixed stuff, but involves a circle
// ----------------------------------------------------------
void ColReaction::solidCollisionOBBxCIRCLE(
    const glm::vec2 &center1, //
    glm::vec2 &vel1,          //
    const glm::vec2 &halfSize1,
    const glm::mat2 &normalizedRotMat1, //
    const glm::vec2 &center2,           //
    glm::vec2 &vel2,                    //
    float radius2,                      //
    float mass1, float mass2)
{
  // Transform circle center into OBB local space
  const glm::vec2 diff = center2 - center1;
  const glm::vec2 local = glm::transpose(normalizedRotMat1) * diff;

  // Clamp to OBB extents
  const glm::vec2 clamped = glm::clamp(local, -halfSize1, halfSize1);

  // Closest point back to world space
  const glm::vec2 closest = center1 + normalizedRotMat1 * clamped;

  // Normal from closest point to circle center
  const glm::vec2 normal = center2 - closest;
  basicImpulse(vel1, vel2, normal, mass1, mass2);
}

// Solid collision between static AABB and Circle shape
void ColReaction::solidCollisionStaticAABBxCIRCLE(glm::vec2 &center1,       //
                                                  glm::vec2 &vel1,          //
                                                  float radius1,            //
                                                  const glm::vec2 &center2, //
                                                  const glm::vec2 &halfSize2)
{
  const glm::vec2 diff = center1 - center2;

  // Clamp to AABB extents
  const glm::vec2 closestLocal = glm::clamp(diff, -halfSize2, halfSize2);

  // Convert back to world position
  const glm::vec2 closestPoint = center2 + closestLocal;

  // Normal from closest point to circle center
  const glm::vec2 normal = glm::normalize(center1 - closestPoint);
  basicImpulseStatic(vel1, normal);
}

// Solid collision between a OBB and static Circle shape
void solidCollisionOBBxStaticCIRCLE(const glm::vec2 &center1,           //
                                    glm::vec2 &vel1,                    //
                                    const glm::vec2 &halfSize1,         //
                                    const glm::mat2 &normalizedRotMat1, //
                                    const glm::vec2 &center2,           //
                                    float radius2)
{
  // Transform circle center into OBB local space
  const glm::vec2 diff = center2 - center1;
  const glm::vec2 local = glm::transpose(normalizedRotMat1) * diff;

  // Clamp to OBB extents
  const glm::vec2 clamped = glm::clamp(local, -halfSize1, halfSize1);

  // Closest point back to world space
  const glm::vec2 closest = center1 + normalizedRotMat1 * clamped;

  // Normal from closest point to circle center
  const glm::vec2 normal = center2 - closest;
  basicImpulseStatic(vel1, normal);
}

// Solid collision between a static OBB and Circle shape
void solidCollisionStaticOBBxCIRCLE(const glm::vec2 &center1,           //
                                    const glm::vec2 &halfSize1,         //
                                    const glm::mat2 &normalizedRotMat1, //
                                    const glm::vec2 &center2,           //
                                    glm::vec2 &vel2,                    //
                                    float radius2)
{ // Transform circle center into OBB local space
  const glm::vec2 diff = center2 - center1;
  const glm::vec2 local = glm::transpose(normalizedRotMat1) * diff;

  // Clamp to OBB extents
  const glm::vec2 clamped = glm::clamp(local, -halfSize1, halfSize1);

  // Closest point back to world space
  const glm::vec2 closest = center1 + normalizedRotMat1 * clamped;

  // Normal from closest point to circle center
  const glm::vec2 normal = center2 - closest;
  basicImpulseStatic(vel2, normal);
}
} // namespace pain
