#include "glm/ext/vector_float2.hpp"
// Between same capulses
namespace pain
{

namespace ColDet
{
struct Result {
  bool isDetected;
  float penetration = 0.f;
  glm::vec2 normal = glm::vec2(0.f);
};

// ---------------------------------------------------------------------------
// Different collision detection between same shapes
// ---------------------------------------------------------------------------

// Check detection between two AABBs
// AABB = Axis Aligned Bounding Box
Result checkAABBCollision(const glm::vec2 &pos1, const glm::vec2 &size1,
                          const glm::vec2 &pos2, const glm::vec2 &size2);

// Check detection between two capsules
// Result checkCapsuleCollision(const glm::vec2 &pos1, float height1, float
// radius1,
//                            const glm::vec2 &pos2, float height2, float
//                            radius2);

// Check detection between two circles
Result checkCircleCollision(const glm::vec2 &pos1, float radius1,
                            const glm::vec2 &pos2, float radius2);
// Check detection between two OBBs
// OBB = Oriented Bounding Box
Result checkOBBCollision(const glm::vec2 &center1, const glm::vec2 &halfSize1,
                         const glm::mat2 &normalizedRotMat1, //
                         const glm::vec2 &center2, const glm::vec2 &halfSize2,
                         const glm::mat2 &normalizedRotMat2);

// ---------------------------------------------------------------------------
// Between different shapes n * (n-1)/2 cases. Most likely 3 or 6 or 10
// ---------------------------------------------------------------------------

Result checkAABBCollisionOBB(const glm::vec2 &pos1, const glm::vec2 &size1,
                             const glm::vec2 &center2,
                             const glm::vec2 &halfSize2,
                             const glm::mat2 &normalizedRotMat2);
// Result checkCapsuleCircleCollision(const glm::vec2 &pos1, float height1,
//                                  float radius1, const glm::vec2 &pos2,
//                                  float height2, float radius2);
Result checkAABBCollisionCircle(const glm::vec2 &pos1, const glm::vec2 &size1,
                                const glm::vec2 &circleCenter, float radius2);
Result checkOBBCollisionCircle(const glm::vec2 &center1,
                               const glm::vec2 &halfSize1,
                               const glm::mat2 &normalizedRotMat1,
                               const glm::vec2 &pos2, float radius2);
// Result checkAABBCapsuleCollision(const glm::vec2 &pos1, float height1,
//                                float radius1, const glm::vec2 &pos2,
//                                float height2, float radius2);
} // namespace ColDet
} // namespace pain
