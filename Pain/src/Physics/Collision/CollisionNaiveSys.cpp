#include "Physics/Collision/CollisionNaiveSys.h"

#include "Debugging/Profiling.h"
#include "Physics/Collision/ColDetection.h"
#include "Physics/Collision/Collider.h"
#include "Scripting/CollisionCallback.h"
#include "Scripting/Component.h"
namespace pain
{
namespace Systems
{
// =============================================================
// SOLID COLLISION REACTION
// =============================================================
void circleVsStaticAABB(glm::vec3 &pos1, glm::vec3 &vel1, float radius,
                        const glm::vec3 &boxPos, const glm::vec2 &halfSize)
{
  // 1. Find closest point on the AABB to the circle
  glm::vec3 closestPoint;
  closestPoint.x =
      glm::clamp(pos1.x, boxPos.x - halfSize.x, boxPos.x + halfSize.x);
  closestPoint.y =
      glm::clamp(pos1.y, boxPos.y - halfSize.y, boxPos.y + halfSize.y);
  closestPoint.z = pos1.z; // 2D

  // 2. Vector from closest point to circle center
  glm::vec3 delta = pos1 - closestPoint;
  float dist2 = glm::dot(delta, delta);

  // No collision
  if (dist2 >= radius * radius)
    return;

  // 3. Collision normal
  glm::vec3 normal;
  if (dist2 == 0.0f)
    normal = glm::vec3(0, 1, 0); // arbitrary
  else
    normal = glm::normalize(delta);

  // 4. Velocity reflection (static object → infinite mass)
  float v1n = glm::dot(vel1, normal);
  if (v1n < 0.0f)
    vel1 -= 2.0f * v1n * normal;

  // 5. Positional correction (push circle out)
  float penetration = radius - std::sqrt(dist2);
  pos1 += normal * penetration;
}
void solidCollision(glm::vec3 &pos1, //
                    glm::vec3 &vel1, //
                    glm::vec3 &pos2, //
                    glm::vec3 &vel2, //
                    float mass1 = 1.f, float mass2 = 1.f)
{
  glm::vec3 normal = glm::normalize(pos2 - pos1);

  // vel along normal
  float v1n = glm::dot(vel1, normal);
  float v2n = glm::dot(vel2, normal);

  // small check objects are moving apart e.g. collision already made
  if (v1n - v2n > 0.0f)
    return;

  // not sure if its correct I just use gpt
  // --- Perfectly elastic collision response (1D along the normal) ---
  float newV1n = (v1n * (mass1 - mass2) + 2.0f * mass2 * v2n) / (mass1 + mass2);
  float newV2n = (v2n * (mass2 - mass1) + 2.0f * mass1 * v1n) / (mass1 + mass2);

  // --- Compute velocity changes along the normal ---
  glm::vec3 deltaV1 = (newV1n - v1n) * normal;
  glm::vec3 deltaV2 = (newV2n - v2n) * normal;

  vel1 += deltaV1;
  vel2 += deltaV2;
};
void solidCollisionStatic(glm::vec3 &pos1, glm::vec3 &vel1,
                          const glm::vec3 &pos2)
{
  // NOTE: I'm assuming this is basically valid for circles
  // Normal from object 1 → object 2
  glm::vec3 normal = glm::normalize(pos2 - pos1);

  // Velocity component along the collision normal
  float v1n = glm::dot(vel1, normal);

  // If moving away, no collision response needed
  if (v1n > 0.0f)
    return;

  // Perfectly elastic bounce off an immovable object:
  // Reverse the normal component of velocity
  vel1 -= 2.0f * v1n * normal;
}
// =============================================================
// COLLISION DETECTION
// =============================================================
// Between same capulses
void NaiveCollisionSystem::onUpdate(double deltaTime)
{

  // NOTE: testing without callbacks for now
  {
    PROFILE_SCOPE("Scene::updateSystems - collision detection");
    auto [tIt_i, cIt_i, mIt_i] =
        begin<TransformComponent, ColliderComponent, MovementComponent>();
    auto [tIt_j, cIt_j, mIt_j] =
        begin<TransformComponent, ColliderComponent, MovementComponent>();
    auto [tItEnd, cItEnd, mItEnd] =
        end<TransformComponent, ColliderComponent, MovementComponent>();

    for (; tIt_i != tItEnd; ++tIt_i, ++cIt_i, ++mIt_i) {

      for (auto tIt_j = tIt_i + 1; tIt_j != tItEnd; ++tIt_j, ++cIt_j, ++mIt_j) {
        glm::vec3 pos1 = tIt_i->m_position + glm::vec3(cIt_i->m_offset, 0.f);
        glm::vec3 pos2 = tIt_j->m_position + glm::vec3(cIt_j->m_offset, 0.f);
        bool collisionHappened = false;

        std::visit(
            [&](auto &&shape1, auto &&shape2) {
              using T1 = std::decay_t<decltype(shape1)>;
              using T2 = std::decay_t<decltype(shape2)>;
              if constexpr (std::is_same_v<T1, AABBShape> &&
                            std::is_same_v<T2, AABBShape>) {

                collisionHappened = ColDet::checkAABBCollision(
                    pos1, shape1.halfSize, pos2, shape2.halfSize);
              } else if constexpr (std::is_same_v<T1, CirleShape> &&
                                   std::is_same_v<T2, CirleShape>) {
                collisionHappened = ColDet::checkCircleCollision(
                    pos1, shape1.radius, pos2, shape2.radius);
              } else if constexpr (std::is_same_v<T1, CirleShape> &&
                                   std::is_same_v<T2, AABBShape>) {
                collisionHappened = ColDet::checkAABBCollisionCircle(
                    pos2, shape2.halfSize, pos1, shape1.radius);
              }
            },
            cIt_i->m_shape, cIt_j->m_shape);

        // --- Perform Collision Reaction on Entities with Collision Scripts
        if (collisionHappened) {
          if (cIt_i->m_isTrigger || cIt_j->m_isTrigger) {
            // TODO: perform trigger callback
            PLOG_I("Trigger collision happened despite being WIP");
          } else {
            solidCollision(pos1,              //
                           mIt_i->m_velocity, //
                           pos2,              //
                           mIt_j->m_velocity  //
            );
          }
        }
      }
    }
  }
  {
    PROFILE_SCOPE(
        "Scene::updateSystems - collision detection vs static objects");
    auto [tIt_i, cIt_i, mIt_i] =
        begin<TransformComponent, ColliderComponent, MovementComponent>();
    auto [tIt_j, cIt_j] = begin<TransformComponent, ColliderComponent>();
    auto [tItEnd, cItEnd, mItEnd] =
        end<TransformComponent, ColliderComponent, MovementComponent>();

    for (; tIt_i != tItEnd; ++tIt_i, ++cIt_i, ++mIt_i) {

      for (auto tIt_j = tIt_i + 1; tIt_j != tItEnd; ++tIt_j, ++cIt_j) {
        glm::vec3 pos1 = tIt_i->m_position + glm::vec3(cIt_i->m_offset, 0.f);
        glm::vec3 pos2 = tIt_j->m_position + glm::vec3(cIt_j->m_offset, 0.f);
        bool collisionHappened = false;

        std::visit(
            [&](auto &&shape1, auto &&shape2) {
              using T1 = std::decay_t<decltype(shape1)>;
              using T2 = std::decay_t<decltype(shape2)>;
              if constexpr (std::is_same_v<T1, AABBShape> &&
                            std::is_same_v<T2, AABBShape>) {

                collisionHappened = ColDet::checkAABBCollision(
                    pos1, shape1.halfSize, pos2, shape2.halfSize);
              } else if constexpr (std::is_same_v<T1, CirleShape> &&
                                   std::is_same_v<T2, CirleShape>) {
                collisionHappened = ColDet::checkCircleCollision(
                    pos1, shape1.radius, pos2, shape2.radius);
              } else if constexpr (std::is_same_v<T1, CirleShape> &&
                                   std::is_same_v<T2, AABBShape>) {
                collisionHappened = ColDet::checkAABBCollisionCircle(
                    pos2, shape2.halfSize, pos1, shape1.radius);
              }
            },
            cIt_i->m_shape, cIt_j->m_shape);

        // --- Perform Collision Reaction on Entities with Collision Scripts
        if (collisionHappened) {
          if (cIt_i->m_isTrigger || cIt_j->m_isTrigger) {
            // TODO: perform trigger callback
            PLOG_I("Trigger collision happened despite being WIP");
          } else {
            solidCollisionStatic(pos1,              //
                                 mIt_i->m_velocity, //
                                 pos2               //
            );
          }
        }
      }
    }
  }

  // // Step 3: Detect collisions for entities with callbacks
  // {
  //   PROFILE_SCOPE("Scene::updateSystems - collision detection");
  //
  //   // Get all entities with colliders
  //   std::vector<std::tuple<TransformComponent *, ColliderComponent *,
  //   Entity>>
  //       allColliders;
  //
  //   auto [tIt, cIt] = begin<TransformComponent, ColliderComponent>();
  //   auto [tItEnd, cItEnd] = end<TransformComponent, ColliderComponent>();
  //   for (; tIt != tItEnd; ++tIt, ++cIt) {
  //     allColliders.push_back({&(*tIt), &(*cIt), tIt.getEntity()});
  //   }
  //
  //   // Check entities that have callbacks for collisions
  //   auto [calTr, calCo, calCCC] = begin<TransformComponent,
  //   ColliderComponent,
  //                                       CollisionCallbackComponent>();
  //   auto [calTrEnd, calCoEnd, calCCCEnd] =
  //       end<TransformComponent, ColliderComponent,
  //           CollisionCallbackComponent>();
  //
  //   for (; callTr != tIt2End; ++tIt2, ++cIt2, ++ccIt) {
  //     glm::vec3 pos1 = tIt2->m_position + glm::vec3(cIt2->m_offset, 0.0f);
  //     Entity selfId = tIt2.getEntity();
  //
  //     // Query nearby entities from spatial grid
  //     auto nearby = m_gridManager.query(tIt2->m_position, cIt2->m_size);
  //
  //     for (size_t idx : nearby) {
  //       if (idx >= allColliders.size())
  //         continue;
  //
  //       auto &[otherTransform, otherCollider, otherId] = allColliders[idx];
  //       if (selfId == otherId)
  //         continue; // Skip self
  //
  //       glm::vec3 pos2 = otherTransform->m_position +
  //                        glm::vec3(otherCollider->m_offset, 0.0f);
  //
  //       if (checkAABBCollision(pos1, cIt2->m_size, pos2,
  //                              otherCollider->m_size)) {
  //         ccIt->addCurrentCollision(otherId);
  //       }
  //     }
  //   }
  // }
  // // NOTE: if you are thinking this is slow because of thet getEntity,
  // remember
  // // that this only happens when a collision happens, which is (hopefully)
  // very
  // // little
  // {
  //   PROFILE_SCOPE("Scene::updateSystems - collision callbacks");
  //
  //   for (auto it = begin<CollisionCallbackComponent>();
  //        it != end<CollisionCallbackComponent>(); ++it) {
  //     CollisionCallbackComponent &ccc = *it;
  //     if (!ccc.instance)
  //       continue;
  //
  //     // Check for new collisions (Enter)
  //     if (ccc.onCollisionEnterFunction) {
  //       for (Entity other : ccc.m_currentCollisions) {
  //         if (!ccc.wasCollidingLastFrame(other)) {
  //           ccc.onCollisionEnterFunction(ccc.instance, other);
  //         }
  //       }
  //     }
  //
  //     // Check for ongoing collisions (Stay)
  //     if (ccc.onCollisionStayFunction) {
  //       for (Entity other : ccc.m_currentCollisions) {
  //         if (ccc.wasCollidingLastFrame(other)) {
  //           ccc.onCollisionStayFunction(ccc.instance, other);
  //         }
  //       }
  //     }
  //
  //     // Check for ended collisions (Exit)
  //     if (ccc.onCollisionExitFunction) {
  //       for (Entity other : ccc.m_previousCollisions) {
  //         if (!ccc.isCollidingThisFrame(other)) {
  //           ccc.onCollisionExitFunction(ccc.instance, other);
  //         }
  //       }
  //     }
  //   }
  // }
  // {
  //   PROFILE_SCOPE("Scene::updateSystems - collision Lua callbacks");
  //   for (auto it = begin<LuaScriptComponent>(); it !=
  //   end<LuaScriptComponent>();
  //        ++it) {
  //     LuaScriptComponent &lsc = *it;
  //     // Check collisions
  //     if (lsc.m_onCollisionEnter) {
  //       for (Entity other : lsc.m_currentCollisions) {
  //         sol::protected_function_result result =
  //             (*lsc.m_onCollisionEnter)(lsc, other);
  //         if (!result.valid()) {
  //           PLOG_E("Lua error (m_onRenderFunction): {}",
  //                  result.get<sol::error>().what());
  //         }
  //       }
  //     }
  //   }
  // }
} // namespace Systems

} // namespace pain
