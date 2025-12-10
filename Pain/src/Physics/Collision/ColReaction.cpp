#include "Physics/Collision/ColReaction.h"
#include "CoreFiles/LogWrapper.h"
// Some of the stuff was taking from here
// https://github.com/OneLoneCoder/Javidx9/blob/0c8ec20a9ed3b2daf76a925034ac5e7e6f4096e0/ConsoleGameEngine/BiggerProjects/Balls/OneLoneCoder_Balls1.cpp#L205

namespace pain
{
constexpr float percent = 0.5f; // correction strength (50%)
// constexpr float slop = 0.01f;   // small tolerance

void basicImpulse(glm::vec2 &center1, //
                  glm::vec2 &vel1,    //
                  float mass1,        //
                  glm::vec2 &center2, //
                  glm::vec2 &vel2,    //
                  float mass2,        //
                  const glm::vec2 normal, float penetration,
                  float restitution = 1.f)
{
  const float invMass1 = 1.f / mass1;
  const float invMass2 = 1.f / mass2;
  // -------------------------------------------------
  // 1. POSITIONAL CORRECTION (using penetration)
  // -------------------------------------------------
  float correctionMag =
      std::max(penetration, 0.0f) / (invMass1 + invMass2) * percent;

  // fix normal direction to be center1 -> center2 direction
  const glm::vec2 correctNormal =
      glm::dot(center2 - center1, normal) < 0 ? -normal : normal;

  glm::vec2 correction = correctionMag * correctNormal;

  center1 -= correction * invMass1;
  center2 += correction * invMass2;

  // -------------------------------------------------
  // 2. Velocity correction (using penetration)
  // -------------------------------------------------
  const glm::vec2 relVel = vel2 - vel1;

  // If the objects are already moving away, cancel
  const float relNormVel = glm::dot(relVel, correctNormal);
  if (relNormVel > 0.f)
    return;

  // NOTE: restitution ∈ [0,1]
  float j = -(1.f + restitution) * relNormVel;
  j /= (invMass1 + invMass2);

  const glm::vec2 impulse = j * correctNormal;
  vel1 -= impulse * invMass1;
  vel2 += impulse * invMass2;
}

void basicImpulseStatic(glm::vec2 &center1, glm ::vec2 &vel1,
                        const glm::vec2 &center2, const glm::vec2 normal,
                        float penetration, const float restitution)
{
  // -------------------------------------------------
  // 1. POSITIONAL CORRECTION (using penetration)
  // -------------------------------------------------
  float correctionMag = std::max(penetration, 0.0f) * percent;

  // fix normal direction to be center1 -> center2 direction
  const glm::vec2 correctNormal =
      glm::dot(center2 - center1, normal) < 0 ? -normal : normal;

  glm::vec2 correction = correctionMag * correctNormal;

  center1 -= correction;
  // -------------------------------------------------
  // 2. Velocity correction (using impulse)
  // -------------------------------------------------
  // Ensure the normal is normalized
  const glm::vec2 relVel = -vel1;

  // If the dynamic object is already moving away, cancel

  const float relNormVel = glm::dot(relVel, correctNormal);
  if (relNormVel > 0.f) {
    return;
  }

  // NOTE: restitution ∈ [0,1]
  float j = -(1.f + restitution) * relNormVel;

  // Apply impulse to velocity
  const glm::vec2 impulse = j * correctNormal;
  vel1 -= impulse;
}

void ColReaction::solidCollisionDynamic(  //
    glm::vec2 &center1,                   //
    glm::vec2 &vel1,                      //
    glm::vec2 &center2,                   //
    glm::vec2 &vel2,                      //
    const glm::vec2 &collisionNormal,     //
    const float penetration,              //
    const float mass1, const float mass2, //
    const float restitution)
{
  basicImpulse(center1, vel1, mass1, center2, vel2, mass2, collisionNormal,
               penetration, restitution);
}

void ColReaction::solidCollisionStatic( //
    glm::vec2 &center1,                 //
    glm::vec2 &vel1,                    //
    const glm::vec2 &center2,           //
    const glm::vec2 &collisionNormal,   //
    const float penetration,            //
    const float restitution             //
)
{
  basicImpulseStatic(center1, vel1, center2, collisionNormal, penetration,
                     restitution);
}
} // namespace pain
