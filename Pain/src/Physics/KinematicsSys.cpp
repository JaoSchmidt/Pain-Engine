/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Physics/KinematicsSys.h"
#include "Debugging/Profiling.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
namespace pain
{

namespace Systems
{
namespace detail
{
void _implOnUpdate(
    DeltaTime deltaTime,
    std::vector<reg::ChunkView<RotationComponent>> chunksRotation,
    std::vector<reg::ChunkView<Transform2dComponent, Movement2dComponent>>
        chunksMovement2d,
    std::vector<reg::ChunkView<Transform3dComponent, Movement3dComponent>>
        chunksMovement3d)
{
  PROFILE_SCOPE("Scene::udpateSystems - Kinematics");
  // =============================================================== //
  // Update Rotation Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - rotation");
    for (auto &chunk : chunksRotation) {
      auto *__restrict r = std::get<0>(chunk.arrays);

      for (size_t i = 0; i < chunk.count; ++i) {
        r[i].m_rotation = {cos(glm::radians(r[i].m_rotationRadians)),
                           sin(glm::radians(r[i].m_rotationRadians)), 0};
      }
    }
  }

  // =============================================================== //
  // Update Movement Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - movement");
    float dt = deltaTime.getSecondsf();

    for (auto &chunk : chunksMovement2d) {
      auto *__restrict t = std::get<0>(chunk.arrays);
      auto *__restrict m = std::get<1>(chunk.arrays);

      for (size_t i = 0; i < chunk.count; ++i) {
        t[i].m_position += m[i].m_velocity * dt;
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::updateSystems - movement");
    float dt = deltaTime.getSecondsf();

    for (auto &chunk : chunksMovement3d) {
      auto *__restrict t = std::get<0>(chunk.arrays);
      auto *__restrict m = std::get<1>(chunk.arrays);

      for (size_t i = 0; i < chunk.count; ++i) {
        t[i].m_position += m[i].m_velocity * dt;
      }
    }
  }
}
} // namespace detail

} // namespace Systems
} // namespace pain
