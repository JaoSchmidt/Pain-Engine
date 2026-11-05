/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Physics/Collision/CollisionNaiveSys.h"

#include "Debugging/Profiling.h"
#include "Misc/Events.h"
#include "Physics/Collision/ColDetection.h"
#include "Physics/Collision/ColReaction.h"
#include "Physics/Collision/Collider.h"
namespace pain
{
namespace Systems
{
namespace
{

// =============================================================
// COLLISION DETECTION
// =============================================================
void narrowPhaseCollisionStatic(     //
    Transform2dComponent &t_i,       //
    const ColliderComponent &c_i,    //
    Movement2dComponent &m_i,        //
    const reg::Entity &entity_i,     //
    const Transform2dComponent &t_j, //
    const ColliderComponent &c_j,    //
    const reg::Entity &entity_j,     //
    reg::EventDispatcher &eventDispatcher)
{
  glm::vec2 center1 = t_i.m_position + c_i.m_offset;
  glm::vec2 center2 = t_j.m_position + c_j.m_offset;
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
      c_i.m_shape, c_j.m_shape);

  // --- Perform Collision Reaction on Entities with Collision Scripts
  if (collisionHappened.isDetected) {
    if (c_i.m_isTrigger || c_j.m_isTrigger) {
      eventDispatcher.enqueue<CollisionEvent>({entity_i, entity_j,
                                               collisionHappened.normal,
                                               collisionHappened.penetration});
    } else {
      // PLOG_I("Static Collision between ({},{}) and ({},{})",
      //        TP_VEC2(t_i.m_position), TP_VEC2(t_j.m_position));
      ColReaction::solidCollisionStatic(t_i.m_position,
                                        m_i.m_velocity, //
                                        t_j.m_position, //
                                        collisionHappened.normal,
                                        collisionHappened.penetration);
    }
  }
}
void narrowPhaseCollision(        //
    Transform2dComponent &t_i,    //
    const ColliderComponent &c_i, //
    Movement2dComponent &m_i,     //
    const reg::Entity &entity_i,  //
    Transform2dComponent &t_j,    //
    const ColliderComponent &c_j, //
    Movement2dComponent &m_j,     //
    const reg::Entity &entity_j,  //
    reg::EventDispatcher &eventDispatcher)
{
  glm::vec2 center1 = t_i.m_position + c_i.m_offset;
  glm::vec2 center2 = t_j.m_position + c_j.m_offset;
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
          // NOTE: if your LSP says center2 and center1 are swapped, ignore it
          collisionHappened = ColDet::checkAABBCollisionCircle(
              center2, shape2.halfSize, center1, shape1.radius);
        }
      },
      c_i.m_shape, c_j.m_shape);

  // --- Perform Collision Reaction on Entities with Collision Scripts
  if (collisionHappened.isDetected) {
    if (c_i.m_isTrigger || c_j.m_isTrigger) {
      eventDispatcher.enqueue<CollisionEvent>( //
          {
              entity_i, entity_j,           //
              collisionHappened.normal,     //
              collisionHappened.penetration //
          });
    } else {
      ColReaction::solidCollisionDynamic( //
          t_i.m_position,                 //
          m_i.m_velocity,                 //
          t_j.m_position,                 //
          m_j.m_velocity, collisionHappened.normal,
          collisionHappened.penetration);
    }
  }
}
} // namespace
void detail::_implOnUpdate(
    DeltaTime deltaTime,
    std::vector<reg::ChunkView<Transform2dComponent, ColliderComponent>>
        chunksStatic,
    std::vector<reg::ChunkView<Transform2dComponent, ColliderComponent,
                               Movement2dComponent>>
        chunks,
    reg::EventDispatcher &eventDispatcher)
{
  UNUSED(deltaTime)
  PROFILE_SCOPE("Scene::updateSystems - collision detection")
  {
    PROFILE_SCOPE(
        "Scene::updateSystems - collision detection vs moving objects");
    for (size_t ci = 0; ci < chunks.size(); ++ci) {
      auto &chunk_i = chunks[ci];

      auto *__restrict t_i = std::get<0>(chunk_i.arrays);
      const auto *__restrict c_i = std::get<1>(chunk_i.arrays);
      auto *__restrict m_i = std::get<2>(chunk_i.arrays);
      const reg::Entity *e_i = chunk_i.entities.data();

      for (size_t cj = ci; cj < chunks.size(); ++cj) {
        auto &chunk_j = chunks[cj];

        auto *__restrict t_j = std::get<0>(chunk_j.arrays);
        const auto *__restrict c_j = std::get<1>(chunk_j.arrays);
        auto *__restrict m_j = std::get<2>(chunk_j.arrays);
        const reg::Entity *e_j = chunks[ci].entities.data();

        // Compare against different chunks, i.e: i ∈ chunk_i and j ∈ chunk_j
        const bool same_chunk = (ci == cj);
        for (size_t i = 0; i < chunk_i.count; ++i) {
          // NOTE: If same chunk, start at i+1 to avoid duplicates
          // Why? Bc if there are in the same chunk, then i = j at first
          size_t j_start = same_chunk ? i + 1 : 0;

          for (size_t j = j_start; j < chunk_j.count; ++j) {
            narrowPhaseCollision(t_i[i], c_i[i], m_i[i], e_i[i], //
                                 t_j[j], c_j[j], m_j[j], e_j[j], //
                                 eventDispatcher);
          }
        }
      }
    }
  }
  {
    PROFILE_SCOPE(
        "Scene::updateSystems - collision detection vs static objects");

    for (size_t ci = 0; ci < chunks.size(); ++ci) {
      auto &chunk_i = chunks[ci];

      auto *__restrict t_i = std::get<0>(chunk_i.arrays);
      const auto *__restrict c_i = std::get<1>(chunk_i.arrays);
      auto *__restrict m_i = std::get<2>(chunk_i.arrays);
      const reg::Entity *e_i = chunk_i.entities.data();

      for (size_t cj = 0; cj < chunksStatic.size(); ++cj) {
        auto &chunk_j = chunksStatic[cj];

        auto *__restrict t_j = std::get<0>(chunk_j.arrays);
        const auto *__restrict c_j = std::get<1>(chunk_j.arrays);
        const reg::Entity *e_j = chunk_i.entities.data();

        for (size_t i = 0; i < chunk_i.count; ++i) {
          for (size_t j = 0; j < chunk_j.count; ++j) {
            narrowPhaseCollisionStatic(         //
                t_i[i], c_i[i], m_i[i], e_i[i], //
                t_j[j], c_j[j], e_j[j],         //
                eventDispatcher);
          }
        }
      }
    }
  }
}

} // namespace Systems

} // namespace pain
