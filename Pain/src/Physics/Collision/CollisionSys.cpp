#include "Physics/Collision/CollisionSys.h"

#include "Debugging/Profiling.h"
#include "Physics/Collision/Collider.h"
#include "Scripting/CollisionCallback.h"
#include "Scripting/Component.h"
namespace pain
{
namespace Systems
{

void CollisionSystem::solidCollision(glm::vec3 &pos1, //
                                     glm::vec3 &vel1, //
                                     glm::vec3 &pos2, //
                                     glm::vec3 &vel2, //
                                     float mass1, float mass2)
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

bool checkAABBCollision(const glm::vec3 &pos1, const glm::vec2 &size1,
                        const glm::vec3 &pos2, const glm::vec2 &size2)
{
  // think as pos = left and pos + size = right
  return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x &&
          pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y);
}
void CollisionSystem::onUpdate(double deltaTime)
{
  PROFILE_FUNCTION();

  // NOTE: testing without callbacks for now
  // Step 1: update grid
  {
    PROFILE_SCOPE("Scene::updateSystems - update dynamic spatial partitioning");

    m_gridManager.clearAllDynamics();
    auto [tIt, cIt, mIt] =
        begin<TransformComponent, ColliderComponent, MovementComponent>();
    auto [tItEnd, cItEnd, mItEnd] =
        end<TransformComponent, ColliderComponent, MovementComponent>();

    for (; tIt != tItEnd; ++tIt, ++cIt, ++mIt) {
      m_gridManager.insertDynamic(*cIt, *tIt, *mIt);
    }
  }
  // // Step 2: clear "collision count" from entities with callbacks
  // {
  //   PROFILE_SCOPE("Scene::updateSystems - clear collision frames");
  //   for (auto it = begin<CollisionCallbackComponent>();
  //        it != end<CollisionCallbackComponent>(); ++it) {
  //     it->clearCurrentFrame();
  //   }
  // }
  // Step 3: Detect and react to solid object first
  {
    PROFILE_SCOPE("Scene::updateSystems - collision detection");
    auto [tIt, cIt] = begin<TransformComponent, ColliderComponent>();
    auto [tItEnd, cItEnd] = end<TransformComponent, ColliderComponent>();

    for (const auto &gridIt : m_gridManager.m_grid) {
      const GridCell &cell = gridIt.second;

      auto &components = cell.m_components;

      const std::size_t count = components.size();
      for (std::size_t i = 0; i < count; ++i) {
        for (std::size_t j = i + 1; j < count; ++j) {
          auto &[collider1, transform1, movement1] = components[i];
          auto &[collider2, transform2, movement2] = components[j];

          // --- Compute world-space positions (include offsets) ---
          glm::vec3 pos1 =
              transform1->m_position + glm::vec3(collider1->m_offset, 0.f);
          glm::vec3 pos2 =
              transform2->m_position + glm::vec3(collider2->m_offset, 0.f);

          const bool collisionHappened = checkAABBCollision(
              pos1, collider1->m_size, pos2, collider2->m_size);

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
}

} // namespace Systems

} // namespace pain
