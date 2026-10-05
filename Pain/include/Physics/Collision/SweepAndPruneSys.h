/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file SweepAndPruneSys.h
 * @brief Broad-phase collision detection system using a Sweep-and-Prune
 * algorithm.
 *
 * Sweep And Prune System implements a better collision algorithm than the naive
 * alternativ  *
 * High-level pipeline:
 *  1. Build or update endpoints from collider components.
 *  2. Incrementally sort endpoints using insertion sort for temporal coherence.
 *  3. Sweep along the primary axis (x) to generate potential overlapping pairs.
 *  4. Validate overlaps on the secondary axis (y).
 *  5. Perform narrow-phase collision detection and reaction. Equivalent to
 * naive collision
 *
 * @note As with any systems in the engine, system headers follow the naming
 * convention and end with the suffix "Sys.h".
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *
 * If the interface is removed, the engine will no longer invoke the associated
 * callback.
 */
#pragma once

#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"
#include "Physics/Collision/Collider.h"
#include "Physics/MovementComponent.h"
namespace pain
{

namespace Systems
{
/** @brief EndPoint, will be hidden after c++20 modules  */
struct EndPoint {
  size_t key;
  float valueOnAxis;
  bool isMin;
  // used in the initial sort
};
/** @brief EndPointKey, will be hidden after c++20 modules  */
struct EndPointKey {
  reg::Entity entity;
  size_t index_minX;
  size_t index_maxX;
  size_t index_minY;
  size_t index_maxY;
};

namespace detail
{
void sortSAP(std::vector<EndPoint> &vec, std::vector<EndPointKey> &keyVec,
             bool isX);

void insertionSortEndpoint(std::vector<EndPoint> &endpoints,
                           std::vector<EndPointKey> &keys, size_t index,
                           bool isXAxis);

size_t insertEndPoint(reg::Entity entity, std::vector<EndPoint> &vecX,
                      std::vector<EndPoint> &vecY,
                      std::vector<EndPointKey> &vecKeys,
                      const Transform2dComponent &transComp,
                      SAPCollider &colComp);

void sortAfterInsertion(
    std::vector<EndPoint> &endPointsX,      //
    std::vector<EndPoint> &endPointsY,      //
    std::vector<EndPointKey> &endPointKeys, //
    std::vector<EndPoint> &staticEndPointsX,      //
    std::vector<EndPoint> &staticEndPointsY,      //
    std::vector<EndPointKey> &staticEndPointKeys, //
    bool &firstTime);

long staticMSB(size_t x);
size_t clearMSB(long x);

void _implOnUpdate(
    DeltaTime deltaTime,
    std::vector<reg::ChunkView<Transform2dComponent, SAPCollider,
                               Movement2dComponent>>
        chunksDynamic,
    std::vector<reg::ChunkView<Transform2dComponent, SAPCollider>>
        chunksStatic,
    std::vector<EndPoint> &endPointsX,      //
    std::vector<EndPoint> &endPointsY,      //
    std::vector<EndPointKey> &endPointKeys, //
    std::vector<EndPoint> &staticEndPointsX,      //
    std::vector<EndPoint> &staticEndPointsY,      //
    std::vector<EndPointKey> &staticEndPointKeys, //
    std::vector<long> &activeList,                //
    bool &firstTime,                              //
    reg::EventDispatcher &eventDispatcher);

} // namespace detail

/**
 * @brief Sweep-and-prune collision detection system.
 *
 * This system performs broad-phase collision detection using a sweep-and-prune
 * algorithm, followed by narrow-phase collision testing and reaction. It
 * operates on entities that contain transform, movement, and sweep-and-prune
 * collider components.
 *
 * Dynamic colliders are updated every frame based on their transform and
 * movement. Static colliders are tracked separately and only participate in
 * dynamic-vs-static comparisons.
 *
 * Collision responses may either:
 *  - Dispatch collision events for trigger colliders.
 *  - Apply physical collision reactions for solid colliders.
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *
 * @note The Tags list is validated at compile time to ensure that all required
 * components are registered in the component manager before this system can be
 * added to a scene.
 *
 * @see SAPCollider
 * @see Transform2dComponent
 * @see Movement2dComponent
 * @see System
 * @see IOnUpdate
 */
template <reg::CompileTimeBitMaskType CM>
struct SweepAndPruneSys : public System<CM>, public IOnUpdate {
  /** @brief Inherit base System constructors. */
  using System<CM>::System;
  /**
   * @brief Component tags required by this system.
   *
   * Declares that this system operates on entities containing:
   *  - Transform2dComponent
   *  - Movement2dComponent
   *  - SAPCollider
   */
  using Tags = TypeList<Transform2dComponent, //
                        Movement2dComponent,  //
                        SAPCollider>;
  /** @brief Default construction is disabled. */
  SweepAndPruneSys() = delete;

  /**
   * @brief Performs one update step of the sweep-and-prune pipeline.
   *
   * Updates endpoint projections, performs sweeping to detect potential
   * overlaps, validates overlaps across multiple axes, and executes
   * narrow-phase collision detection and reaction.
   *
   * @param deltaTime Frame delta time.
   *
   * @note This method is invoked only because the system inherits from
   * IOnUpdate.
   */
  void onUpdate(DeltaTime deltaTime) override
  {
    detail::_implOnUpdate(
        deltaTime,
        this->template query<Transform2dComponent, SAPCollider,
                             Movement2dComponent>(),
        this->template query<Transform2dComponent, SAPCollider>(
            exclude<Movement2dComponent>),
        m_endPointsX, m_endPointsY, m_endPointKeys, m_staticEndPointsX,
        m_staticEndPointsY, m_staticEndPointKeys, m_activeList, m_firstTime,
        this->m_eventDispatcher);
  }
  // void deleteStaticEntities(std::span<reg::Entity> entities);
  // void deleteStaticEntity(reg::Entity entitiy);
  // void deleteEntity(reg::Entity entitiy);

  /**
   * @brief Inserts one or more colliders into the sweep structure.
   *
   * Accepts individual entities or containers of entities and inserts their
   * collider endpoints into the internal endpoint lists. After insertion, the
   * endpoint lists are re-sorted to preserve sweep correctness.
   *
   * @tparam Args Entity or container types.
   * @param blob One or more entities or containers of entities.
   */
  template <typename... Args> void insertColliders(const Args &...blob)
  {
    auto deBlob = [this](const auto &blob) {
      using E = std::remove_const_t<std::remove_cvref_t<decltype(blob)>>;

      if constexpr (std::same_as<E, reg::Entity>) {
        insertCollider(blob);
      } else if constexpr (std::same_as<E, std::vector<reg::Entity>>) {
        insertColliderSpan(blob);
      }
    };
    (deBlob(blob), ...);
    detail::sortAfterInsertion(m_endPointsX, m_endPointsY, m_endPointKeys,
                               m_staticEndPointsX, m_staticEndPointsY,
                               m_staticEndPointKeys, m_firstTime);
  }

  /**
   * @brief Inserts a single collider entity into the sweep structure.
   *
   * Retrieves the required components from the entity and computes its initial
   * endpoint projections.
   *
   * @param entity Target entity.
   * @return Index of the newly inserted endpoint key.
   */
  size_t insertCollider(reg::Entity entity)
  {
    auto [tc, sc] =
        this->template getComponents<Transform2dComponent, SAPCollider>(entity);
    return insertColliderDirectly(entity, tc, sc);
  }

  /**
   * @brief Inserts a collider directly using provided component references.
   *
   * This overload avoids repeated component lookups and allows tighter control
   * when inserting large batches of entities.
   *
   * @param entity Target entity.
   * @param tc Transform component reference.
   * @param sc Collider component reference.
   * @return Index of the newly inserted endpoint key.
   */
  size_t insertColliderDirectly(reg::Entity entity,
                                const Transform2dComponent &tc,
                                SAPCollider &sc)
  {
    const bool isDynamic =
        this->template hasAnyComponents<Movement2dComponent>(entity);

    auto &endPointsX = isDynamic ? m_endPointsX : m_staticEndPointsX;
    auto &endPointsY = isDynamic ? m_endPointsY : m_staticEndPointsY;
    auto &endPointKeys = isDynamic ? m_endPointKeys : m_staticEndPointKeys;

    return detail::insertEndPoint(entity, endPointsX, endPointsY, endPointKeys,
                                  tc, sc);
  }

  /**
   * @brief Inserts multiple collider entities at once.
   *
   * @param entities List of entities to insert.
   */
  void insertColliderSpan(const std::vector<reg::Entity> &entities)
  {
    for (reg::Entity entity : entities)
      insertCollider(entity);
  }

private:
  std::vector<EndPoint> m_endPointsX = {};
  std::vector<EndPoint> m_endPointsY = {};

  std::vector<EndPoint> m_staticEndPointsX = {};
  std::vector<EndPoint> m_staticEndPointsY = {};

  std::vector<EndPointKey> m_endPointKeys = {};
  std::vector<EndPointKey> m_staticEndPointKeys = {};

  std::vector<long> m_activeList = {};
  std::vector<long> m_activeStaticList = {};

  bool m_firstTime = true;
};

} // namespace Systems
} // namespace pain