#include "Physics/KinematicsSys.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"
namespace pain
{

namespace Systems
{
void Systems::Kinematics::onUpdate(DeltaTime deltaTime)
{
  PROFILE_FUNCTION();
  // =============================================================== //
  // Update Rotation Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - rotation");
    for (auto rIt = begin<RotationComponent>(); rIt != end<RotationComponent>();
         ++rIt) {
      rIt->m_rotation = {cos(rIt->m_rotationAngle), sin(rIt->m_rotationAngle),
                         0};
    }
  }

  // =============================================================== //
  // Update Movement Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - movement");
    auto [tIt, mIt] = begin<Transform2dComponent, Movement2dComponent>();
    auto [tItEnd, mItEnd] = end<Transform2dComponent, Movement2dComponent>();
    for (; tIt != tItEnd; ++tIt, ++mIt) {
      tIt->m_position += mIt->m_velocity * deltaTime.getSecondsf();
    }
  }
}

} // namespace Systems
} // namespace pain
