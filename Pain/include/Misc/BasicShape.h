/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file BasicShape.h
 * @brief Lightweight geometric shape definitions used by collision systems.
 *
 * Defines simple POD shape structures used for collision detection,
 * spatial queries, and debug visualization. These types are typically
 * embedded in collider components or physics systems.
 */

#pragma once

#include "Core.h"
#include "glm/ext/vector_float2.hpp"

namespace pain
{

/**
 * @brief Circular collision shape.
 *
 * Represents a circle centered at an entity's transform position,
 * optionally offset by a collider component.
 */
struct CircleShape {
  float radius = 0.1f; /**< Radius of the circle in world units. */
};

/**
 * @brief Axis-Aligned Bounding Box (AABB) shape.
 *
 * The box is centered on the entity position and defined using half extents.
 */
struct AABBShape {
  glm::vec2 halfSize = {0.1f,
                        0.1f}; /**< Half-size extents along X and Y axes. */
};

/**
 * @brief Quad shape defined by full size.
 *
 * Mainly used for rendering or simple shape representation rather than
 * strict collision math.
 */
struct QuadShape {
  glm::vec2 size = {0.1f, 0.1f}; /**< Full size of the rectangle. */
};

/**
 * @brief Capsule collision shape.
 *
 * A capsule is composed of a central segment with two semicircular ends.
 * Useful for character controllers and smooth collision behavior.
 */
struct CapsuleShape {
  float height = 0.1f; /**< Height of the central segment. */
  float radius = 0.1f; /**< Radius of the capsule ends. */
};

/**
 * @brief Polygon collision shape (not implemented).
 *
 * Placeholder for future polygon collision support.
 */
struct PolygonShape {
  // TODO: not implemented yet
};

} // namespace pain
