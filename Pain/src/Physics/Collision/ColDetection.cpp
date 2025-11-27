// ColDetection.cpp
#include "Physics/Collision/ColDetection.h"
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

bool ColDet::checkAABBCollision(const glm::vec2 &pos1, const glm::vec2 &size1,
                                const glm::vec2 &pos2, const glm::vec2 &size2)
{
  // think as pos = left and pos + size = right
  return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x &&
          pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y);
}

bool ColDet::checkCircleCollision(const glm::vec2 &pos1, float radius1,
                                  const glm::vec2 &pos2, float radius2)
{
  // think as pos = left and pos + size = right
  return glm::distance(pos1, pos2) < radius1 + radius2;
}
bool ColDet::checkAABBCollisionCircle(const glm::vec2 &pos1,
                                      const glm::vec2 &size1,
                                      const glm::vec2 &circleCenter2,
                                      float radius2)
{
  glm::vec2 closestPoint = glm::clamp(circleCenter2, pos1, pos1 + size1);
  return glm::distance(circleCenter2, closestPoint) <= radius2;
}
bool ColDet::checkAABBCollisionOBB( // AABB
    const glm::vec2 &pos1, const glm::vec2 &size1,
    // OBB
    const glm::vec2 &center2, const glm::vec2 &halfSize2,
    const glm::mat2 &normalizedRotMat2)
{

  const std::array<glm::vec2, 4> aabbCorners = {
      pos1,                         // top left
      pos1 + glm::vec2(size1.x, 0), // top right
      pos1 + size1,                 // bottom right
      pos1 + glm::vec2(0, size1.y), // bottom left
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
  const float minAABBx = pos1.x;
  const float maxAABBx = pos1.x + size1.x;
  const float minAABBy = pos1.y;
  const float maxAABBy = pos1.y + size1.y;

  // ----- SAT Implemented -----
  {
    // axis global x
    const auto [minOBBx, maxOBBx] = projectionMinMax<4>({1, 0}, obbCorners);
    if (maxAABBx < minOBBx || minAABBx > maxOBBx)
      return false;
    // axis global y
    const auto [minOBBy, maxOBBy] = projectionMinMax<4>({0, 1}, obbCorners);
    if (maxAABBy < minOBBy || minAABBy > maxOBBy)
      return false;
  }

  // axis OBB projected x
  {
    const auto [minOBBx, maxOBBx] =
        projectionMinMax<4>(normalizedRotMat2 * glm::vec2(1, 0), obbCorners);
    const auto [minAABBx, maxAABBx] =
        projectionMinMax<4>(normalizedRotMat2 * glm::vec2(1, 0), aabbCorners);
    if (maxAABBx < minOBBx || minAABBx > maxOBBx)
      return false;
  }
  // axis OBB projected y
  {
    const auto [minOBBy, maxOBBy] =
        projectionMinMax<4>(normalizedRotMat2 * glm::vec2(0, 1), obbCorners);
    const auto [minAABBy, maxAABBy] =
        projectionMinMax<4>(normalizedRotMat2 * glm::vec2(0, 1), obbCorners);
    if (maxAABBy < minOBBy || minAABBy > maxOBBy)
      return false;
  }
  return true;
}

bool ColDet::checkOBBCollision(const glm::vec2 &center1,
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
  for (glm::vec2 axis : all_axes) {
    auto [min1, max1] = projectionMinMax<4>(axis, obbCorners1);
    auto [min2, max2] = projectionMinMax<4>(axis, obbCorners2);

    if (max1 < min2 || min1 > max2)
      return false;
  }

  return true;
}

bool ColDet::checkOBBCollisionCircle(const glm::vec2 &center1,
                                     const glm::vec2 &halfSize1,
                                     const glm::mat2 &normalizedRotMat1,
                                     const glm::vec2 &circleCenter2,
                                     float radius2)
{
  glm::mat2 invR = glm::transpose(normalizedRotMat1);
  glm::vec2 circleLocal = invR * (circleCenter2 - center1);
  glm::vec2 closestLocal = glm::clamp(circleLocal, -halfSize1, halfSize1);
  return glm::distance(circleLocal, closestLocal) <= radius2;
}

} // namespace pain
