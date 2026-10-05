/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file Collider.h
 * @brief Collision components for narrow-phase and sweep-and-prune systems.
 *
 * Defines two collider components:
 *
 * - ColliderComponent:
 *     Lightweight ECS collider description used by gameplay and physics
 * systems.
 *
 * - SAPCollider:
 *     Collider representation used by the Sweep-And-Prune broad-phase system.
 *     Stores additional indexing data and supports direct registration into
 *     the broad-phase accelerator.
 */

#pragma once

#include "CoreRender/Shapes.h"
#include "ECS/Components/ComponentManager.h"
#include <variant>

namespace pain
{

/**
 * @brief Basic collider component attached to an entity.
 *
 * Represents the collision shape and local offset relative to the entity
 * transform. This component does not manage any spatial acceleration data.
 *
 * The shape is stored as a variant and may be:
 * - CircleShape
 * - AABBShape
 * - CapsuleShape
 */
struct ColliderComponent {
  using tag = tag::Collider;

  glm::vec2 m_offset{0.0F,
                     0.0F}; /**< Local offset from the entity transform. */
  std::variant<CircleShape, AABBShape, CapsuleShape> m_shape{
      AABBShape{}};        /**< Collision shape. */
  bool m_isTrigger{false}; /**< If true, collider generates events but does not
                              resolve physics. */

  /**
   * @brief Creates an AABB collider component.
   *
   * @param size Full size of the box.
   * @param offset Local offset from the transform.
   * @param isTrigger Whether the collider acts as a trigger.
   */
  static ColliderComponent createAABB(const glm::vec2 &size = {0.1F, 0.1F},
                                      const glm::vec2 &offset = {0.0F, 0.0F},
                                      bool isTrigger = false);

  /**
   * @brief Creates a circular collider component.
   *
   * @param radius Circle radius.
   * @param offset Local offset from the transform.
   * @param isTrigger Whether the collider acts as a trigger.
   */
  static ColliderComponent createCircle(float radius,
                                        const glm::vec2 &offset = {0.0F, 0.0F},
                                        bool isTrigger = false);

  /**
   * @brief Creates a capsule collider component.
   *
   * @param capsuleHeight Height of the capsule cylinder section.
   * @param capsuleSemiCircleRadius Radius of the capsule hemispheres.
   * @param offset Local offset from the transform.
   * @param isTrigger Whether the collider acts as a trigger.
   */
  static ColliderComponent createCapsule(float capsuleHeight,
                                         float capsuleSemiCircleRadius,
                                         const glm::vec2 &offset = {0.0F, 0.0F},
                                         bool isTrigger = false);
};

/**
 * @brief Sweep-And-Prune collider representation.
 *
 * This component stores additional bookkeeping information required by the
 * Sweep-And-Prune broad-phase collision system. It mirrors the collider shape
 * and offset while maintaining an internal index used by the accelerator.
 *
 * Static colliders may be registered immediately into the system at creation.
 */
struct SAPCollider {
  using tag = tag::SAPCollider;

  glm::vec2 m_offset{0.0F,
                     0.0F}; /**< Local offset from the entity transform. */
  std::variant<CircleShape, AABBShape, CapsuleShape> m_shape{
      AABBShape{}};        /**< Collision shape. */
  bool m_isTrigger{false}; /**< If true, collider generates events but does not
                              resolve physics. */
  int m_index{-1}; /**< Internal index used by the Sweep-And-Prune system. */

  // ------------------------------------------------------------
  // Deferred creation (not inserted into Sweep-And-Prune)
  // ------------------------------------------------------------

  /**
   * @brief Creates an AABB SAP collider without registering it in the system.
   *
   * If the object is static, the caller is responsible for inserting the
   * collider into the Sweep-And-Prune system later.
   *
   * @param size Full size of the box.
   * @param isTrigger Whether the collider acts as a trigger.
   * @param offset Local offset from the transform.
   */
  static SAPCollider createAABB(const glm::vec2 &size = {0.1F, 0.1F},
                                bool isTrigger = false,
                                const glm::vec2 &offset = {0.0F, 0.0F});

  /**
   * @brief Creates a circular SAP collider without registering it in the
   * system.
   *
   * If the object is static, the caller is responsible for inserting the
   * collider into the Sweep-And-Prune system later.
   *
   * @param radius Circle radius.
   * @param isTrigger Whether the collider acts as a trigger.
   * @param offset Local offset from the transform.
   */
  static SAPCollider createCircle(float radius, bool isTrigger = false,
                                  const glm::vec2 &offset = {0.0F, 0.0F});
};

} // namespace pain
