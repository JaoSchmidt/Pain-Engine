#include "Physics/Kinematics.h"
#include "Debugging/Profiling.h"
namespace pain
{

namespace Systems
{
void Systems::Kinematics::onUpdate(double deltaTime)
{
  PROFILE_FUNCTION();
  // =============================================================== //
  // Update Rotation Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - rotation");
    for (auto it = begin<RotationComponent>(); it != end<RotationComponent>();
         ++it) {
      RotationComponent &rc = *it;
      rc.m_rotation = {cos(rc.m_rotationAngle), sin(rc.m_rotationAngle), 0};
    }
  }

  // =============================================================== //
  // Update Movement Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - movement");
    auto [tIt, mIt] = begin<TransformComponent, MovementComponent>();
    auto [tItEnd, mItEnd] = end<TransformComponent, MovementComponent>();
    for (; tIt != tItEnd; ++tIt, ++mIt) {
      const float moveAmount = (float)(mIt->m_translationSpeed * deltaTime);
      tIt->m_position += mIt->m_velocityDir * moveAmount;
    }
  }
}

} // namespace Systems
} // namespace pain
