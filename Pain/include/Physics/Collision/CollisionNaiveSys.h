/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file NaiveCollisionSys.h
 * @brief Simple brute-force collision detection and response system.
 *
 * NaiveCollisionSys performs pairwise collision checks between all relevant
 * entities each update tick. It is intended primarily for debugging,
 * prototyping, and small scenes where correctness and simplicity are more
 * important than performance.
 *
 * The system executes:
 *  - Broad iteration over all entities containing Transform2dComponent,
 *    Movement2dComponent, and ColliderComponent.
 *  - Narrow-phase collision tests between collider shapes.
 *  - Basic collision response by modifying movement state.
 *
 * Because this implementation scales quadratically with entity count, it is
 * not suitable for large scenes or production-scale physics workloads.
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
#include "ECS/Systems.h"
#include "Physics/Collision/Collider.h"
#include "Physics/MovementComponent.h"

namespace pain
{
namespace Systems
{

/**
 * @brief Brute-force collision detection system for dynamic and static
 * entities.
 *
 * NaiveCollisionSys iterates through all entities that contain the required
 * components and performs collision checks between every valid pair. Depending
 * on whether both entities are dynamic or one is static, different narrow-phase
 * resolution paths are executed.
 *
 * This system is primarily useful as:
 *  - A correctness reference for more optimized collision systems.
 *  - A lightweight solution for very small worlds.
 *  - A debugging tool during engine development.
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *
 * @note The Tags list is validated at compile time to ensure that all required
 * components are registered in the component manager before this system can be
 * added to a scene.
 *
 * @see Transform2dComponent
 * @see Movement2dComponent
 * @see ColliderComponent
 * @see System
 * @see IOnUpdate
 */
struct NaiveCollisionSys : public System<WorldComponents>, IOnUpdate {
  /**
   * @brief Component tags required by this system.
   *
   * Declares that this system operates on entities containing:
   *  - Transform2dComponent
   *  - Movement2dComponent
   *  - ColliderComponent
   */
  using Tags = TypeList<Transform2dComponent, //
                        Movement2dComponent,  //
                        ColliderComponent>;

  /** @brief Inherit base System constructors. */
  using System<WorldComponents>::System;

  /** @brief Prevent default construction. */
  NaiveCollisionSys() = delete;

  /**
   * @brief Performs collision detection and resolution for the current frame.
   *
   * Iterates through all valid entity pairs and executes narrow-phase collision
   * tests and response logic.
   *
   * @param dt Frame delta time.
   *
   * @note This method is invoked only because the system inherits from
   * IOnUpdate.
   */
  void onUpdate(DeltaTime dt) override;

private:
  /**
   * @brief Resolves collision between a dynamic entity and a static entity.
   *
   * Performs narrow-phase collision detection between a moving entity and a
   * static collider, and applies the appropriate response to the dynamic
   * movement component.
   *
   * @param t_i        Transform of the dynamic entity.
   * @param c_i        Collider of the dynamic entity.
   * @param m_i        Movement of the dynamic entity.
   * @param entity_i  Entity identifier of the dynamic entity.
   * @param t_j        Transform of the static entity.
   * @param c_j        Collider of the static entity.
   * @param entity_j  Entity identifier of the static entity.
   */
  void narrowPhaseCollisionStatic(Transform2dComponent &t_i,
                                  ColliderComponent &c_i,
                                  Movement2dComponent &m_i,
                                  reg::Entity entity_i,
                                  const Transform2dComponent &t_j,
                                  const ColliderComponent &c_j,
                                  reg::Entity entity_j);

  /**
   * @brief Resolves collision between two dynamic entities.
   *
   * Performs narrow-phase collision detection and applies response logic to
   * both participating entities.
   *
   * @param t_i        Transform of the first entity.
   * @param c_i        Collider of the first entity.
   * @param m_i        Movement of the first entity.
   * @param entity_i  Entity identifier of the first entity.
   * @param t_j        Transform of the second entity.
   * @param c_j        Collider of the second entity.
   * @param m_j        Movement of the second entity.
   * @param entity_j  Entity identifier of the second entity.
   */
  void narrowPhaseCollision(Transform2dComponent &t_i, ColliderComponent &c_i,
                            Movement2dComponent &m_i, reg::Entity entity_i,
                            Transform2dComponent &t_j, ColliderComponent &c_j,
                            Movement2dComponent &m_j, reg::Entity entity_j);
};

} // namespace Systems
} // namespace pain
