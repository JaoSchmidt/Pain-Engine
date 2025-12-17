// ColDetection.cpp
#include "Physics/Collision/ColDetection.h"
#include "CoreFiles/LogWrapper.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "glm/ext/vector_float2.hpp"
#include "imgui.h"
#include <utility>
// Between same capulses
namespace pain
{

template <std::size_t... Indexes>
constexpr std::tuple<float, float>
projectionMinMaxImpl(const glm::vec2 &axis,
                     const std::array<glm::vec2, sizeof...(Indexes)> &pts,
                     std::index_sequence<Indexes...>)
{
  // HACK: not really the projection here, just the dot product, i.e., corner *
  // axis. That is, the projection module ONLY, without the directions. If we
  // wanted the directions we multiply by `axis / || axis ||²`
  auto projections = std::array{glm::dot(pts[Indexes], axis)...};
  return {std::min({projections[Indexes]...}),
          std::max({projections[Indexes]...})};
}
// for all the points of polygon, project them into an axis, then get the
// furthest and closest points from the origin
template <std::size_t N>
std::tuple<float, float> projectionMinMax(const glm::vec2 &axis,
                                          const std::array<glm::vec2, N> &pts)
{
  return projectionMinMaxImpl(axis, pts, std::make_index_sequence<N>{});
};

// ---------------------------------------------------------------------------
// Different collision detection between same shapes
// ---------------------------------------------------------------------------

ColDet::Result ColDet::checkAABBCollision(const glm::vec2 &center1,
                                          const glm::vec2 &halfSize1,
                                          const glm::vec2 &center2,
                                          const glm::vec2 &halfSize2)
{
  const float minAABB1x = center1.x - halfSize1.x;
  const float maxAABB1x = center1.x + halfSize1.x;
  const float minAABB1y = center1.y - halfSize1.y;
  const float maxAABB1y = center1.y + halfSize1.y;

  const float minAABB2x = center2.x - halfSize2.x;
  const float maxAABB2x = center2.x + halfSize2.x;
  const float minAABB2y = center2.y - halfSize2.y;
  const float maxAABB2y = center2.y + halfSize2.y;

  if (minAABB1x < maxAABB2x && maxAABB1x > minAABB2x && //
      minAABB1y < maxAABB2y && maxAABB1y > minAABB2y) {
    const glm::vec2 diff = center2 - center1;

    // Overlaps on each axis
    float overlapX =
        std::min(maxAABB1x, maxAABB2x) - std::max(minAABB1x, minAABB2x);
    float overlapY =
        std::min(maxAABB1y, maxAABB2y) - std::max(minAABB1y, minAABB2y);

    // Collision normal = axis of minimum penetration
    glm::vec2 collisionNormal(0.f);
    float overlap = 0;

    if (overlapX < overlapY) { // axis choosen is {1,0}
      overlap = overlapX;
      collisionNormal.x = (diff.x < 0.f) ? -1.f : 1.f;
    } else {
      overlap = overlapY;
      collisionNormal.y = (diff.y < 0.f) ? -1.f : 1.f;
    }

    return {true, overlap, glm::normalize(collisionNormal)};
  } else {
    return {false};
  }
}

ColDet::Result ColDet::checkCircleCollision(const glm::vec2 &center1,
                                            float radius1,
                                            const glm::vec2 &center2,
                                            float radius2)
{
  const float collisionDistance = glm::distance(center1, center2);
  if (collisionDistance < radius1 + radius2) {
    const glm::vec2 normal = center2 - center1;
    const glm::vec2 normalized =
        glm::length(normal) > 0.0f ? glm::normalize(normal) : glm::vec2(0.0f);
    const float overlay = radius1 + radius2 - collisionDistance;
    return {true, overlay, normalized};
  } else {
    return {false};
  }
}

ColDet::Result ColDet::checkAABBCollisionCircle(const glm::vec2 &center1,
                                                const glm::vec2 &halfSize1,
                                                const glm::vec2 &center2,
                                                float radius2)
{
  glm::vec2 diff = center2 - center1;

  glm::vec2 clamped = glm::clamp(diff, -halfSize1, halfSize1);
  glm::vec2 closestPoint = center1 + clamped;

  glm::vec2 normal = center2 - closestPoint;
  float distance = glm::length(normal);

  // Circle center is outside the AABB
  if (distance > 0.0f) {
    if (distance > radius2)
      return {false};

    return {true, radius2 - distance, normal / distance};
  }

  // --- Circle center is INSIDE the AABB ---
  glm::vec2 distancesToEdge = halfSize1 - glm::abs(diff);

  if (distancesToEdge.x < distancesToEdge.y) {
    normal = glm::vec2((diff.x > 0) ? 1.0f : -1.0f, 0.0f);
    return {true, distancesToEdge.x + radius2, normal};
  } else {
    normal = glm::vec2(0.0f, (diff.y > 0) ? 1.0f : -1.0f);
    return {true, distancesToEdge.y + radius2, normal};
  }
}

// ---------------------------------------------------------------------------
// Between different shapes n * (n-1)/2 cases. Most likely 3 or 6 or 10
// ---------------------------------------------------------------------------

ColDet::Result ColDet::checkAABBCollisionOBB( // AABB
    const glm::vec2 &center1, const glm::vec2 &halfSize1,
    // OBB
    const glm::vec2 &center2, const glm::vec2 &halfSize2,
    const glm::mat2 &normalizedRotMat2)
{
  // NOTE: no idea why this isn't working
  // const std::array<glm::vec2, 4> aabbCorners = {
  //     {-halfSize1.x, halfSize1.y}, // top left
  //     halfSize1,                   // top right
  //     {halfSize1.x, -halfSize1.y}, // bottom right
  //     -halfSize1,                  // bottom left
  // };

  const std::array<glm::vec2, 4> aabbCorners = {
      glm::vec2(-halfSize1.x, halfSize1.y), // top left
      halfSize1,                            // top right
      {halfSize1.x, -halfSize1.y},          // bottom right
      -halfSize1,                           // bottom left
  };
  const glm::vec2 obbLocalCorners[4] = {
      {-halfSize2.x, halfSize2.y}, // top left
      halfSize2,                   // top right
      {halfSize2.x, -halfSize2.y}, // bottom right
      -halfSize2,                  // bottom left
  };

  // Corners of OBB at world axis (check your own paper)
  const std::array<glm::vec2, 4> obbCorners = {
      center2 + normalizedRotMat2 * obbLocalCorners[0],
      center2 + normalizedRotMat2 * obbLocalCorners[1],
      center2 + normalizedRotMat2 * obbLocalCorners[2],
      center2 + normalizedRotMat2 * obbLocalCorners[3],
  };

  // For every axis, see if there is an interval that intercepts.
  const float minAABBx = center1.x - halfSize1.x;
  const float maxAABBx = center1.x + halfSize1.x;
  const float minAABBy = center1.y - halfSize1.y;
  const float maxAABBy = center1.y + halfSize1.y;

  float smallestOverlap = std::numeric_limits<float>::max();
  glm::vec2 collisionNormal = glm::vec2(0);
  // ----- SAT Implemented -----
  {
    // axis global x
    const auto [minOBBx, maxOBBx] = projectionMinMax<4>({1, 0}, obbCorners);
    if (maxAABBx < minOBBx || minAABBx > maxOBBx)
      return {false};

    float overlap = std::min(maxAABBx, maxOBBx) - std::max(minAABBx, minOBBx);
    if (overlap < smallestOverlap) {
      smallestOverlap = overlap;
      collisionNormal = {1, 0};
    }
  }

  {
    // axis global y
    const auto [minOBBy, maxOBBy] = projectionMinMax<4>({0, 1}, obbCorners);
    if (maxAABBy < minOBBy || minAABBy > maxOBBy)
      return {false};

    float overlap = std::min(maxAABBy, maxOBBy) - std::max(minAABBy, minOBBy);
    if (overlap < smallestOverlap) {
      smallestOverlap = overlap;
      collisionNormal = {0, 1};
    }
  }

  const glm::vec2 all_axes[2] = {
      glm::normalize(normalizedRotMat2 *
                     glm::vec2(1, 0)), // first testing x axis from OBB1
      glm::normalize(normalizedRotMat2 *
                     glm::vec2(0, 1)), // then testing y axis from OBB1
  };
  // axis OBB projected x and y
  for (glm::vec2 axis : all_axes) {
    auto [min1, max1] = projectionMinMax<4>(axis, obbCorners);
    auto [min2, max2] = projectionMinMax<4>(axis, aabbCorners);

    if (max1 < min2 || min1 > max2)
      return {false};

    float overlap = std::min(max1, max2) - std::max(min1, min2);

    if (overlap < smallestOverlap) {
      smallestOverlap = overlap;
      collisionNormal = axis;
    }
  }
  return {true, smallestOverlap, glm::normalize(collisionNormal)};
}

ColDet::Result ColDet::checkOBBCollision(const glm::vec2 &center1,
                                         const glm::vec2 &halfSize1,
                                         const glm::mat2 &normalizedRotMat1, //
                                         const glm::vec2 &center2,
                                         const glm::vec2 &halfSize2,
                                         const glm::mat2 &normalizedRotMat2)
{
  // clang-format off
  const std::array<glm::vec2, 4> obbCorners1 = {
      center1 + normalizedRotMat1 * glm::vec2{-halfSize1.x,  halfSize1.y}, // top left
      center1 + normalizedRotMat1 * glm::vec2{ halfSize1.x,  halfSize1.y}, // top right
      center1 + normalizedRotMat1 * glm::vec2{ halfSize1.x, -halfSize1.y}, // bottom right
      center1 + normalizedRotMat1 * glm::vec2{-halfSize1.x, -halfSize1.y}, // bottom left
  };

  const std::array<glm::vec2, 4> obbCorners2 = {
      center2 + normalizedRotMat2 * glm::vec2{-halfSize2.x,  halfSize2.y}, // top left
      center2 + normalizedRotMat2 * glm::vec2{ halfSize2.x,  halfSize2.y}, // top right
      center2 + normalizedRotMat2 * glm::vec2{ halfSize2.x, -halfSize2.y}, // bottom right
      center2 + normalizedRotMat2 * glm::vec2{-halfSize2.x, -halfSize2.y}, // bottom left
  };


  // ----- SAT Implemented -----
  const glm::vec2 all_axes[4] = {
      glm::normalize(normalizedRotMat1 * glm::vec2(1, 0)), // first testing x axis from OBB1
      glm::normalize(normalizedRotMat1 * glm::vec2(0, 1)), // then testing y axis from OBB1
      glm::normalize(normalizedRotMat2 * glm::vec2(1, 0)), // then testing x axis from OBB2
      glm::normalize(normalizedRotMat2 * glm::vec2(0, 1))}; // then testing y axis from OBB2

  // clang-format on
  // HACK: Minkowski Sum = the normal we want
  float smallestOverlap = std::numeric_limits<float>::max();
  glm::vec2 collisionNormal = glm::vec2(0);

  // detecting
  for (glm::vec2 axis : all_axes) {
    auto [min1, max1] = projectionMinMax<4>(axis, obbCorners1);
    auto [min2, max2] = projectionMinMax<4>(axis, obbCorners2);

    if (max1 < min2 || min1 > max2)
      return {false};

    float overlap = std::min(max1, max2) - std::max(min1, min2);

    if (overlap < smallestOverlap) {
      smallestOverlap = overlap;
      collisionNormal = axis;
    }
  }

  return {true, smallestOverlap, glm::normalize(collisionNormal)};
}

ColDet::Result
ColDet::checkOBBCollisionCircle(const glm::vec2 &center1,
                                const glm::vec2 &halfSize1,
                                const glm::mat2 &normalizedRotMat1, //
                                const glm::vec2 &center2, float radius2)
{
  glm::vec2 localDiff = normalizedRotMat1 * (center2 - center1);
  glm::vec2 closestLocal = glm::clamp(localDiff, -halfSize1, halfSize1);
  glm::vec2 closestPoint = center2 + closestLocal;
  const float collisionDistance = glm::distance(center2, closestPoint);
  if (collisionDistance <= radius2) {
    return {true, std::abs(radius2 - collisionDistance),
            glm::normalize(center2 - closestPoint)};
  } else {
    return {false};
  }
}

} // namespace pain
