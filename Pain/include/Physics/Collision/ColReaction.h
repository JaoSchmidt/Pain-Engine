#include "glm/ext/vector_float2.hpp"
// Between same capulses
namespace pain
{
namespace ColDet
{
// AABB = Axis Aligned Bounding Box
// OBB = Oriented Bounding Box
bool solidCollisionAABB(const glm::vec2 &pos1, const glm::vec2 &vel1,
                        const glm::vec2 &pos2, const glm::vec2 &vel2,
                        float mass1 = 1.f, float mass2 = 1.f);
bool solidCollisionAABBStatic(const glm::vec2 &pos1, const glm::vec2 &vel1,
                              const glm::vec2 &pos2, const glm::vec2 &vel2,
                              float mass1 = 1.f, float mass2 = 1.f);
// bool checkCapsuleCollision(const glm::vec2 &pos1, float height1, float
// radius1,
//                            const glm::vec2 &pos2, float height2, float
//                            radius2);
bool solidCollisionCircle(const glm::vec2 &pos1, float radius1,
                          const glm::vec2 &pos2, float radius2);
bool solidCollisionOBB(const glm::vec2 &center1, const glm::vec2 &halfvel1,
                       const glm::mat2 &normalizedRotMat1, //
                       const glm::vec2 &center2, const glm::vec2 &halfvel2,
                       const glm::mat2 &normalizedRotMat2);

// Between different capulses: n * (n-1)/2 cases. Most likely 3 or 6 or 10
bool solidCollisionAABBxOBB(const glm::vec2 &pos1, const glm::vec2 &vel1,
                            const glm::vec2 &center2, const glm::vec2 &halfvel2,
                            const glm::mat2 &normalizedRotMat2);
// bool checkCapsuleCircleCollision(const glm::vec2 &pos1, float height1,
//                                  float radius1, const glm::vec2 &pos2,
//                                  float height2, float radius2);
bool solidCollisionAABBxCircle(const glm::vec2 &pos1, const glm::vec2 &vel1,
                               const glm::vec2 &circleCenter, float radius2);
bool solidCollisiionOBBxCircle(const glm::vec2 &center1,
                               const glm::vec2 &halfvel1,
                               const glm::mat2 &normalizedRotMat1,
                               const glm::vec2 &pos2, float radius2);
// bool checkAABBCapsuleCollision(const glm::vec2 &pos1, float height1,
//                                float radius1, const glm::vec2 &pos2,
//                                float height2, float radius2);
} // namespace ColDet
} // namespace pain
