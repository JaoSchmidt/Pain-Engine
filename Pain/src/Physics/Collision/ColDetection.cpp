
#include "Physics/Collision/ColDetection.h"
// Between same capulses
namespace pain
{

namespace ColDet
{

bool checkAABBCollision(const glm::vec3 &pos1, const glm::vec2 &size1,
                        const glm::vec3 &pos2, const glm::vec2 &size2)
{
  // think as pos = left and pos + size = right
  return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x &&
          pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y);
}
bool checkCapsuleCollision(const glm::vec3 &pos1, float height1, float radius1,
                           const glm::vec3 &pos2, float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}
bool checkCircleCollision(const glm::vec3 &pos1, float radius1,
                          const glm::vec3 &pos2, float radius2)
{
  // think as pos = left and pos + size = right
  return glm::distance(pos1, pos2) < radius1 + radius2;
}
// Between different capulses: n * (n-1)/2 cases. Most likely 3 or 6 or 10
bool checkCapsuleCircleCollision(const glm::vec3 &pos1, float height1,
                                 float radius1, const glm::vec3 &pos2,
                                 float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}
bool checkAABBCircleCollision(const glm::vec3 &pos1, float height1,
                              float radius1, const glm::vec3 &pos2,
                              float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}
bool checkAABBCapsuleCollision(const glm::vec3 &pos1, float height1,
                               float radius1, const glm::vec3 &pos2,
                               float height2, float radius2)
{
  // think as pos = left and pos + size = right
  return false;
}

} // namespace ColDet
} // namespace pain
