/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file Transform2dComponent.h
 * @brief Basic 2D transform and movement components.
 *
 * Defines lightweight ECS components used to represent an entity's
 * position and movement state in 2D space.
 */

#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "pch.h"

namespace pain
{

/**
 * @brief 2D transform component storing position.
 *
 * Represents the world or local position of an entity in 2D space.
 * This component provides implicit conversion operators to glm::vec2
 * for convenience when interfacing with math and rendering systems.
 */
struct Transform3dComponent {
  using tag = tag::Transform3d;

  glm::vec3 m_position{0.f, 0.f, 0.f}; /**< Position in 3D space. */

  /**
   * @brief Implicit conversion to a mutable glm::vec2 reference.
   *
   * Allows the component to be used directly as a vector reference.
   */
  operator glm::vec3 &() { return m_position; }

  /** @brief Implicit conversion to a constant glm::vec2 reference. */
  operator const glm::vec3 &() const { return m_position; }

  /** @brief Creates a copy of this transform component. */
  Transform3dComponent clone() const { return *this; }
};

/**
 * @brief 2D movement component storing velocity and rotation speed.
 *
 * Represents linear velocity and angular speed applied to an entity
 * during movement or physics updates.
 */
struct Movement3dComponent {
  using tag = tag::Movement3d;

  glm::vec3 m_velocity{0.f, 0.f, 0.f}; /**< Linear velocity in 3D space. */
  float m_rotationSpeed{0.0f};         /**< Angular rotation speed. */

  /** @brief Creates a copy of this transform component. */
  Movement3dComponent clone() const { return *this; }
};

} // namespace pain
