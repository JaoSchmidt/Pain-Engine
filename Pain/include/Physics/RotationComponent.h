/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file RotationComponent.h
 * @brief Component representing rotational state of an entity.
 */

#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "pch.h"

namespace pain
{

/**
 * @brief Stores rotation parameters for an entity.
 *
 * Defines the rotation angle and axis used by rendering or transformation
 * systems to orient an entity in world space.
 */
struct RotationComponent {
  using tag = tag::Rotation;

  float m_rotationAngle{0.0f}; /**< Rotation angle, typically in radians. */
  glm::vec3 m_rotation{0.0f, 1.0f, 0.0f}; /**< Rotation axis vector. */
};

} // namespace pain
