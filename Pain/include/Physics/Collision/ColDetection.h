#include "glm/ext/vector_float2.hpp"
// Between same capulses
namespace pain
{
namespace ColDet
{
// AABB = Axis Aligned Bounding Box
// OBB = Oriented Bounding Box
bool checkAABBCollision(const glm::vec2 &pos1, const glm::vec2 &size1,
                        const glm::vec2 &pos2, const glm::vec2 &size2);
// bool checkCapsuleCollision(const glm::vec2 &pos1, float height1, float
// radius1,
//                            const glm::vec2 &pos2, float height2, float
//                            radius2);
bool checkCircleCollision(const glm::vec2 &pos1, float radius1,
                          const glm::vec2 &pos2, float radius2);
bool checkOBBCollision(const glm::vec2 &center1, const glm::vec2 &halfSize1,
                       const glm::mat2 &normalizedRotMat1, //
                       const glm::vec2 &center2, const glm::vec2 &halfSize2,
                       const glm::mat2 &normalizedRotMat2);

// Between different capulses: n * (n-1)/2 cases. Most likely 3 or 6 or 10
bool checkAABBCollisionOBB(const glm::vec2 &pos1, const glm::vec2 &size1,
                           const glm::vec2 &center2, const glm::vec2 &halfSize2,
                           const glm::mat2 &normalizedRotMat2);
// bool checkCapsuleCircleCollision(const glm::vec2 &pos1, float height1,
//                                  float radius1, const glm::vec2 &pos2,
//                                  float height2, float radius2);
bool checkAABBCollisionCircle(const glm::vec2 &pos1, const glm::vec2 &size1,
                              const glm::vec2 &circleCenter, float radius2);
bool checkOBBCollisionCircle(const glm::vec2 &center1,
                             const glm::vec2 &halfSize1,
                             const glm::mat2 &normalizedRotMat1,
                             const glm::vec2 &pos2, float radius2);
// bool checkAABBCapsuleCollision(const glm::vec2 &pos1, float height1,
//                                float radius1, const glm::vec2 &pos2,
//                                float height2, float radius2);
} // namespace ColDet
} // namespace pain
