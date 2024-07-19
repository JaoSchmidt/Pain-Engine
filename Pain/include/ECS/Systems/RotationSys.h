#include "pch.h"

namespace pain
{
struct RotationSystem {
  static void update(std::unordered_map<Entity, RotationComponent> &map)
  {
    for (auto &pair : map) {
      RotationComponent &rc = pair.second;
      rc.m_rotation = {cos(glm::radians(rc.m_rotationAngle)),
                       sin(glm::radians(rc.m_rotationAngle)), 0};
    }
  }
};

} // namespace pain
