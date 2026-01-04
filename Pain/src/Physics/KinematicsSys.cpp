#include "Physics/KinematicsSys.h"
#include "Debugging/Profiling.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
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
    auto chunks = query<RotationComponent>();

    for (auto &chunk : chunks) {
      auto *__restrict r = std::get<0>(chunk.arrays);

      for (size_t i = 0; i < chunk.count; ++i) {
        r[i].m_rotation = {cos(r[i].m_rotationAngle), sin(r[i].m_rotationAngle),
                           0};
      }
    }
  }

  // =============================================================== //
  // Update Movement Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - movement");
    auto chunks = query<Transform2dComponent, Movement2dComponent>();
    float dt = deltaTime.getSecondsf();

    for (auto &chunk : chunks) {
      auto *__restrict t = std::get<0>(chunk.arrays);
      auto *__restrict m = std::get<1>(chunk.arrays);

      for (size_t i = 0; i < chunk.count; ++i) {
        t[i].m_position += m[i].m_velocity * dt;
      }
    }
  }
}

} // namespace Systems
} // namespace pain
