#include "Physics/Collision/CollisionNaiveSys.h"

#include "Debugging/Profiling.h"
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
bool checkAABBCollision(const glm::vec3 &pos1, const glm::vec2 &size1,
                        const glm::vec3 &pos2, const glm::vec2 &size2)
{
  // think as pos = left and pos + size = right
  return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x &&
          pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y);
}
bool checkCapsuleCollision(const glm::vec3 &pos1, float height1, float radius1,
                           const glm::vec3 &pos2, float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}
bool checkCircleCollision(const glm::vec3 &pos1, float radius1,
                          const glm::vec3 &pos2, float radius2)
{
  // think as pos = left and pos + size = right
  return glm::distance(pos1, pos2) < radius1 + radius2;
}
// Between different capulses: n * (n-1)/2 cases. Most likely 3 or 6 or 10
bool checkCapsuleCircleCollision(const glm::vec3 &pos1, float height1,
                                 float radius1, const glm::vec3 &pos2,
                                 float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}
bool checkAABBCircleCollision(const glm::vec3 &pos1, float height1,
                              float radius1, const glm::vec3 &pos2,
                              float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}
bool checkAABBCapsuleCollision(const glm::vec3 &pos1, float height1,
                               float radius1, const glm::vec3 &pos2,
                               float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}

void CollisionSystem::onUpdate(double deltaTime)
{

  // NOTE: testing without callbacks for now

  auto [tIt, cIt, mIt] =
      begin<TransformComponent, ColliderComponent, MovementComponent>();
  auto [tItEnd, cItEnd, mItEnd] =
      end<TransformComponent, ColliderComponent, MovementComponent>();

  for (; tIt != tItEnd; ++tIt, ++cIt, ++mIt) {
  }

  // Step 3: Detect and react to solid object first
  PROFILE_SCOPE("Scene::updateSystems - collision detection");
  // auto [tIt, cIt] = begin<TransformComponent, ColliderComponent>();
  // auto [tItEnd, cItEnd] = end<TransformComponent, ColliderComponent>();

  const std::size_t count = m_movingObjects.size();
  for (std::size_t i = 0; i < count; ++i) {
    for (std::size_t j = i + 1; j < count; ++j) {
      auto &[collider1, transform1, movement1] = m_movingObjects[i];
      auto &[collider2, transform2, movement2] = m_movingObjects[j];

      // --- Compute world-space positions (include offsets) ---
      glm::vec3 pos1 =
          transform1->m_position + glm::vec3(collider1->m_offset, 0.f);
      glm::vec3 pos2 =
          transform2->m_position + glm::vec3(collider2->m_offset, 0.f);

      bool collisionHappened = false;

      // --- Perform Collision detection

      std::visit(
          [&](auto &&shape1, auto &&shape2) {
            using T1 = std::decay_t<decltype(shape1)>;
            using T2 = std::decay_t<decltype(shape2)>;
            if constexpr (std::is_same_v<T1, AABBShape> &&
                          std::is_same_v<T2, AABBShape>) {

              collisionHappened =
                  checkAABBCollision(pos1, shape1.size, pos2, shape2.size);
            } else if constexpr (std::is_same_v<T1, CapsuleShape> &&
                                 std::is_same_v<T2, CapsuleShape>) {
              collisionHappened =
                  checkAABBCollision(pos1, shape1.size, pos2, shape2.size);
            } else if constexpr (std::is_same_v<T1, CirleShape> &&
                                 std::is_same_v<T2, CirleShape>) {
              collisionHappened =
                  checkAABBCollision(pos1, shape1.size, pos2, shape2.size);
            } else if constexpr (std::is_same_v<T1, AABBShape> &&
                                 std::is_same_v<T2, CapsuleShape>) {
              collisionHappened =
                  checkAABBCollision(pos1, shape1.size, pos2, shape2.size);
            } else if constexpr (std::is_same_v<T1, CapsuleShape> &&
                                 std::is_same_v<T2, AABBShape>) {
              collisionHappened =
                  checkAABBCollision(pos1, shape1.size, pos2, shape2.size);
            }
          },
          collider1->m_shape, collider2->m_shape);

      // --- Perform Collision Reaction on Entities with Collision Scripts
      if (collisionHappened) {
        if (collider1->m_isTrigger || collider2->m_isTrigger) {
          // TODO: perform trigger callback
          PLOG_I("Trigger collision happened despite being WIP");
        } else {
          solidCollision(pos1,                  //
                         movement1->m_velocity, //
                         pos2,                  //
                         movement2->m_velocity  //
          );
        }
      }
    }
  }

  // Collision detection on static objects
  const std::size_t countStatic = m_staticObjects.size();
  for (std::size_t i = 0; i < count; ++i) {
    for (std::size_t j = 0; j < countStatic; ++j) {
      auto &[collider1, transform1, movement1] = m_movingObjects[i];
      auto &[collider2, transform2] = m_staticObjects[j];

      // --- Compute world-space positions (include offsets) ---
      glm::vec3 pos1 =
          transform1->m_position + glm::vec3(collider1->m_offset, 0.f);
      glm::vec3 pos2 =
          transform2->m_position + glm::vec3(collider2->m_offset, 0.f);

      const bool collisionHappened =
          checkAABBCollision(pos1, collider1->m_size, pos2, collider2->m_size);

      // --- Perform Collision Reaction on Entities with Collision Scripts
      if (collisionHappened) {
        if (collider1->m_isTrigger || collider2->m_isTrigger) {
          // TODO: perform trigger callback
          PLOG_I("Trigger collision happened despite being WIP");
        } else {
          solidCollisionStatic(pos1,                  //
                               movement1->m_velocity, //
                               pos2                   //
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
