#include "Physics/Collision/CollisionNaiveSys.h"

#include "Debugging/Profiling.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "Physics/Collision/ColDetection.h"
#include "Physics/Collision/ColReaction.h"
#include "Physics/Collision/Collider.h"
#include "Scripting/CollisionCallback.h"
namespace pain
{
namespace Systems
{

// =============================================================
// COLLISION DETECTION
// =============================================================
// Between same capulses
void NaiveCollisionSys::onUpdate(DeltaTime deltaTime)
{
  UNUSED(deltaTime)
  // NOTE: testing without callbacks for now
  // NOTE: also, testing without rotation for now too. If you introduce rotation
  // in this system, it will break
  {
    PROFILE_SCOPE("Scene::updateSystems - collision detection");
    auto [tIt_i, cIt_i, mIt_i] =
        begin<Transform2dComponent, ColliderComponent, Movement2dComponent>();
    auto [tItEnd, cItEnd, mItEnd] =
        end<Transform2dComponent, ColliderComponent, Movement2dComponent>();

    for (; tIt_i != tItEnd; ++tIt_i, ++cIt_i, ++mIt_i) {
      auto tIt_j = tIt_i + 1;
      auto cIt_j = cIt_i + 1;
      auto mIt_j = mIt_i + 1;
      for (; tIt_j != tItEnd; ++tIt_j, ++cIt_j, ++mIt_j) {
        glm::vec2 center1 = tIt_i->m_position + cIt_i->m_offset;
        glm::vec2 center2 = tIt_j->m_position + cIt_j->m_offset;
        ColDet::Result collisionHappened = {false};

        // -------------------------------------------------------------
        // Collision detection on movable vs movable objects
        // -------------------------------------------------------------
        std::visit(
            [&](auto &&shape1, auto &&shape2) {
              using T1 = std::decay_t<decltype(shape1)>;
              using T2 = std::decay_t<decltype(shape2)>;
              if constexpr (std::is_same_v<T1, AABBShape> &&
                            std::is_same_v<T2, AABBShape>) {
                collisionHappened = ColDet::checkAABBCollision(
                    center1, shape1.halfSize, center2, shape2.halfSize);
              } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                   std::is_same_v<T2, CircleShape>) {
                collisionHappened = ColDet::checkCircleCollision(
                    center1, shape1.radius, center2, shape2.radius);
              } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                   std::is_same_v<T2, AABBShape>) {
                collisionHappened = ColDet::checkAABBCollisionCircle(
                    center2, shape2.halfSize, center1, shape1.radius);
              }
            },
            cIt_i->m_shape, cIt_j->m_shape);

        // --- Perform Collision Reaction on Entities with Collision Scripts
        if (collisionHappened.isDetected) {
          if (cIt_i->m_isTrigger || cIt_j->m_isTrigger) {
            // TODO: perform trigger callback
            PLOG_I("Trigger collision happened despite being WIP");
          } else {
            ColReaction::solidCollisionDynamic( //
                tIt_i->m_position,              //
                mIt_i->m_velocity,              //
                tIt_j->m_position,              //
                mIt_j->m_velocity, collisionHappened.normal,
                collisionHappened.penetration);
          }
        }
      }
    }
  }
  {
    PROFILE_SCOPE(
        "Scene::updateSystems - collision detection vs static objects");
    auto [tIt_i, cIt_i, mIt_i] =
        begin<Transform2dComponent, ColliderComponent, Movement2dComponent>();
    auto [tItEnd, cItEnd, mItEnd] =
        end<Transform2dComponent, ColliderComponent, Movement2dComponent>();
    auto [tIt_jEnd, cIt_jEnd] = end<Transform2dComponent, ColliderComponent>(
        exclude<Movement2dComponent>);

    for (; tIt_i != tItEnd; ++tIt_i, ++cIt_i, ++mIt_i) {
      auto [tIt_j, cIt_j] = begin<Transform2dComponent, ColliderComponent>(
          exclude<Movement2dComponent>);
      for (; tIt_j != tIt_jEnd; ++tIt_j, ++cIt_j) {
        glm::vec2 center1 = tIt_i->m_position + cIt_i->m_offset;
        glm::vec2 center2 = tIt_j->m_position + cIt_j->m_offset;
        ColDet::Result collisionHappened = {false};

        // -------------------------------------------------------------
        // Collision detection on movable vs static objects
        // -------------------------------------------------------------
        std::visit(
            [&](auto &&shape1, auto &&shape2) {
              using T1 = std::decay_t<decltype(shape1)>;
              using T2 = std::decay_t<decltype(shape2)>;
              if constexpr (std::is_same_v<T1, AABBShape> &&
                            std::is_same_v<T2, AABBShape>) {
                collisionHappened = ColDet::checkAABBCollision(
                    center1, shape1.halfSize, center2, shape2.halfSize);
              } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                   std::is_same_v<T2, CircleShape>) {
                collisionHappened = ColDet::checkCircleCollision(
                    center1, shape1.radius, center2, shape2.radius);
              } else if constexpr (std::is_same_v<T1, CircleShape> &&
                                   std::is_same_v<T2, AABBShape>) {
                collisionHappened = ColDet::checkAABBCollisionCircle(
                    center2, shape2.halfSize, center1, shape1.radius);
              }
            },
            cIt_i->m_shape, cIt_j->m_shape);

        // --- Perform Collision Reaction on Entities with Collision Scripts
        if (collisionHappened.isDetected) {
          if (cIt_i->m_isTrigger || cIt_j->m_isTrigger) {
            // TODO: perform trigger callback
            PLOG_I("Trigger collision happened despite being WIP");
          } else {
            // PLOG_I("Static Collision between ({},{}) and ({},{})",
            //        TP_VEC2(tIt_i->m_position), TP_VEC2(tIt_j->m_position));
            ColReaction::solidCollisionStatic(tIt_i->m_position,
                                              mIt_i->m_velocity, //
                                              tIt_j->m_position, //
                                              collisionHappened.normal,
                                              collisionHappened.penetration);
          }
        }
      }
    }
  }
}

} // namespace Systems

} // namespace pain
