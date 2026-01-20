/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "glm/ext/vector_float2.hpp"

/** Narrow-phase collision detection utilities used by naive collision systems.
 * Provides shape-vs-shape intersection tests and minimal penetration data.
 *
 * These functions are stateless and perform pure geometric overlap queries.
 * Returned normals and penetration depths can be used by higher-level
 * resolution systems. */

namespace pain::ColDet
{

/** Represents the result of a narrow-phase collision test. */
struct Result {
  /** Indicates whether a collision was detected. */
  bool isDetected;
  /** Penetration depth between the two shapes when a collision occurs. */
  float penetration = 0.f;
  /** Collision normal pointing from the first shape toward the second. */
  glm::vec2 normal = glm::vec2(0.f);
};

// ---------------------------------------------------------------------------
// Different collision detection between same shapes
// ---------------------------------------------------------------------------

/** Performs collision detection between two axis-aligned bounding boxes (AABB).
 */
/** AABB parameters are expressed using center position and half-size extents.
 */
Result checkAABBCollision(const glm::vec2 &pos1, const glm::vec2 &size1,
                          const glm::vec2 &pos2, const glm::vec2 &size2);

// Check detection between two capsules
// Result checkCapsuleCollision(const glm::vec2 &pos1, float height1, float
// radius1,
//                            const glm::vec2 &pos2, float height2, float
//                            radius2);

/** @brief Performs collision detection between two circles. */
Result checkCircleCollision(const glm::vec2 &pos1, float radius1,
                            const glm::vec2 &pos2, float radius2);

/** Performs collision detection between two oriented bounding boxes (OBB).
 * Rotation is provided as a normalized 2x2 rotation matrix. */
Result checkOBBCollision(const glm::vec2 &center1, const glm::vec2 &halfSize1,
                         const glm::mat2 &normalizedRotMat1, //
                         const glm::vec2 &center2, const glm::vec2 &halfSize2,
                         const glm::mat2 &normalizedRotMat2);

// ---------------------------------------------------------------------------
// Between different shapes n * (n-1)/2 cases. Most likely 3 or 6 or 10
// ---------------------------------------------------------------------------

/** Performs collision detection between an AABB and an OBB. */
Result checkAABBCollisionOBB(const glm::vec2 &pos1, const glm::vec2 &size1,
                             const glm::vec2 &center2,
                             const glm::vec2 &halfSize2,
                             const glm::mat2 &normalizedRotMat2);

// Result checkCapsuleCircleCollision(const glm::vec2 &pos1, float height1,
//                                  float radius1, const glm::vec2 &pos2,
//                                  float height2, float radius2);

/** Performs collision detection between an AABB and a circle. */
Result checkAABBCollisionCircle(const glm::vec2 &pos1, const glm::vec2 &size1,
                                const glm::vec2 &circleCenter, float radius2);

/** Performs collision detection between an OBB and a circle. */
Result checkOBBCollisionCircle(const glm::vec2 &center1,
                               const glm::vec2 &halfSize1,
                               const glm::mat2 &normalizedRotMat1,
                               const glm::vec2 &pos2, float radius2);

// Result checkAABBCapsuleCollision(const glm::vec2 &pos1, float height1,
//                                float radius1, const glm::vec2 &pos2,
//                                float height2, float radius2);

} // namespace pain::ColDet
