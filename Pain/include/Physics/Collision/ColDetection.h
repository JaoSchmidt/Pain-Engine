#include "glm/ext/vector_float2.hpp"
// Between same capulses
namespace pain
{
namespace ColDet
{
bool checkAABBCollision(const glm::vec3 &pos1, const glm::vec2 &size1,
                        const glm::vec3 &pos2, const glm::vec2 &size2);
bool checkCapsuleCollision(const glm::vec3 &pos1, float height1, float radius1,
                           const glm::vec3 &pos2, float height2, float radius2);
bool checkCircleCollision(const glm::vec3 &pos1, float radius1,
                          const glm::vec3 &pos2, float radius2);
// Between different capulses: n * (n-1)/2 cases. Most likely 3 or 6 or 10
bool checkCapsuleCircleCollision(const glm::vec3 &pos1, float height1,
                                 float radius1, const glm::vec3 &pos2,
                                 float height2, float radius2);
bool checkAABBCircleCollision(const glm::vec3 &pos1, float height1,
                              float radius1, const glm::vec3 &pos2,
                              float height2, float radius2);
bool checkAABBCapsuleCollision(const glm::vec3 &pos1, float height1,
                               float radius1, const glm::vec3 &pos2,
                               float height2, float radius2);
} // namespace ColDet
} // namespace pain
