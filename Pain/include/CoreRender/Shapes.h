#pragma once

#include "Core.h"
#include <glm/glm.hpp>
#include <variant>

namespace pain
{

/**
 * @brief Circular collision shape.
 *
 * Represents a circle centered at an entity's transform position,
 * optionally offset by a collider component.
 */
struct CircleShape {
  float radius = 0.125f; /**< Radius of the circle in world units. */
};

/**
 * @brief Axis-Aligned Bounding Box (AABB) shape.
 *
 * The box is centered on the entity position and defined using half extents.
 * Mainly used for collision
 */
struct AABBShape {
  glm::vec2 halfSize = {0.125f,
                        0.125f}; /**< Half-size extents along X and Y axes. */
};

/**
 * @brief Rect shape defined by full size.
 *
 * Mainly used for rendering or simple shape representation rather than
 * strict collision math.
 */
struct RectShape {
  glm::vec2 size = {0.125f, 0.125f}; /**< Full size of the rectangle. */
};

/**
 * @brief Quad shape defined by full size.
 *
 * Mainly used for rendering or simple shape representation rather than
 * strict collision math.
 */
struct QuadShape {
  float side = 0.125f; /**< Full size of the quad. */
};

/**
 * @brief Triangle shape defined by it height and base sizes
 *
 * Mainly used for simple stuff
 */
struct TriangleShape {
  float base = 0.125f;   /**< Base of the triangle */
  float height = 0.125f; /**< Height of the triangle */
};

/**
 * @brief Capsule collision shape.
 *
 * A capsule is composed of a central segment with two semicircular ends.
 * Useful for character controllers and smooth collision behavior.
 */
struct CapsuleShape {
  float height = 0.125f; /**< Height of the central segment. */
  float radius = 0.125f; /**< Radius of the capsule ends. */
};

/**
 * @brief Circular collision shape.
 *
 * Represents a circle centered at an entity's transform position,
 * optionally offset by a collider component.
 */
struct LineShape {
  glm::vec2 destination = {0, 0}; /**< Radius of the circle in world units. */
  float thickness = 1.f;
};

/**
 * @brief Polygon collision shape (not implemented).
 *
 * Placeholder for future polygon collision support.
 */
// struct PolygonShape {
// TODO: not implemented yet
// };

using ShapeVariant =
    std::variant<QuadShape, RectShape, TriangleShape, LineShape>;

} // namespace pain
