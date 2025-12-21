#include "Physics/Collision/CollisionNaiveSys.h"

#include "Debugging/Profiling.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "Misc/Events.h"
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
void NaiveCollisionSys::narrowPhaseCollisionStatic(
    Transform2dComponent &t_i, ColliderComponent &c_i, Movement2dComponent &m_i,
    reg::Entity entity_i, //
    const Transform2dComponent &t_j, const ColliderComponent &c_j,
    reg::Entity entity_j)
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
      m_eventDispatcher.enqueue<CollisionEvent>(
          {entity_i, entity_j, collisionHappened.normal,
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

void NaiveCollisionSys::narrowPhaseCollision(
    Transform2dComponent &t_i, ColliderComponent &c_i, Movement2dComponent &m_i,
    reg::Entity entity_i, //
    Transform2dComponent &t_j, ColliderComponent &c_j, Movement2dComponent &m_j,
    reg::Entity entity_j)
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
          collisionHappened = ColDet::checkAABBCollisionCircle(
              center2, shape2.halfSize, center1, shape1.radius);
        }
      },
      c_i.m_shape, c_j.m_shape);

  // --- Perform Collision Reaction on Entities with Collision Scripts
  if (collisionHappened.isDetected) {
    if (c_i.m_isTrigger || c_j.m_isTrigger) {
      m_eventDispatcher.enqueue<CollisionEvent>(
          {entity_i, entity_j, collisionHappened.normal,
           collisionHappened.penetration});
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

void NaiveCollisionSys::onUpdate(DeltaTime deltaTime)
{
  UNUSED(deltaTime)
  // NOTE: testing without callbacks for now
  // NOTE: also, testing without rotation for now too. If you introduce rotation
  // in this system, it will break
  {
    auto chunks =
        query<Transform2dComponent, ColliderComponent, Movement2dComponent>();

    for (size_t ci = 0; ci < chunks.size(); ++ci) {
      auto &chunk_i = chunks[ci];

      auto *t_chunk_i = std::get<0>(chunk_i.arrays);
      auto *c_chunk_i = std::get<1>(chunk_i.arrays);
      auto *m_chunk_i = std::get<2>(chunk_i.arrays);

      for (size_t cj = ci; cj < chunks.size(); ++cj) {
        auto &chunk_j = chunks[cj];

        auto *t_chunk_j = std::get<0>(chunk_j.arrays);
        auto *c_chunk_j = std::get<1>(chunk_j.arrays);
        auto *m_chunk_j = std::get<2>(chunk_j.arrays);

        // Compare against different chunks, i.e: i ∈ chunk_i and j ∈ chunk_j
        const bool same_chunk = (ci == cj);
        for (size_t i = 0; i < chunk_i.count; ++i) {
          // NOTE: If same chunk, start at i+1 to avoid duplicates
          // Why? Bc if there are in the same chunk, then i = j at first
          size_t j_start = same_chunk ? i + 1 : 0;

          for (size_t j = j_start; j < chunk_j.count; ++j) {
            narrowPhaseCollision(
                t_chunk_i[i], c_chunk_i[i], m_chunk_i[i], chunk_i.entities[i],
                t_chunk_j[j], c_chunk_j[j], m_chunk_j[j], chunk_j.entities[j]);
          }
        }
      }
    }
  }
  {
    PROFILE_SCOPE(
        "Scene::updateSystems - collision detection vs static objects");
    auto chunks =
        query<Transform2dComponent, ColliderComponent, Movement2dComponent>();
    auto chunksStatic = query<Transform2dComponent, ColliderComponent>(
        exclude<Movement2dComponent>);

    for (size_t ci = 0; ci < chunks.size(); ++ci) {
      auto &chunk_i = chunks[ci];

      auto *t_chunk_i = std::get<0>(chunk_i.arrays);
      auto *c_chunk_i = std::get<1>(chunk_i.arrays);
      auto *m_chunk_i = std::get<2>(chunk_i.arrays);

      for (size_t cj = 0; cj < chunksStatic.size(); ++cj) {
        auto &chunk_j = chunksStatic[cj];

        auto *t_chunk_j = std::get<0>(chunk_j.arrays);
        auto *c_chunk_j = std::get<1>(chunk_j.arrays);

        for (size_t i = 0; i < chunk_i.count; ++i) {
          for (size_t j = 0; j < chunk_j.count; ++j) {
            narrowPhaseCollisionStatic( //
                t_chunk_i[i], c_chunk_i[i], m_chunk_i[i], chunk_i.entities[i],
                t_chunk_j[j], c_chunk_j[j], chunk_j.entities[j]);
          }
        }
      }
    }
  }
}

} // namespace Systems

} // namespace pain
