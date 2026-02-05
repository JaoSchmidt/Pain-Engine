/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Physics/Particles/ParticleSys.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/Profiling.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
#include "glm/ext/matrix_transform.hpp"

static constexpr glm::mat3 rotate90{glm::vec3(0, -1, 0), glm::vec3(1, 0, 0),
                                    glm::vec3(0, 0, 1)};
namespace pain
{

namespace Systems
{
void Systems::ParticleSys::onUpdate(DeltaTime deltaTime)
{
  auto chunks = query<ParticleSprayComponent>();
  for (auto &chunk : chunks) {
    auto *__restrict pc = std::get<0>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; i++) {
      pc[i].elapsed += deltaTime;
    }
  }
}

void Systems::ParticleSys::onRender(pain::Renderer2d &renderer,
                                    bool isMinimized,
                                    pain::DeltaTime currentTime)
{
  UNUSED(isMinimized)
  PROFILE_FUNCTION();
  // =============================================================== //
  // Update Rotation Components
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::updateSystems - rotation");
    auto chunks = query<Transform2dComponent, ParticleSprayComponent,
                        RotationComponent>();

    for (auto &chunk : chunks) {
      auto *__restrict tc = std::get<0>(chunk.arrays);
      auto *__restrict psc = std::get<1>(chunk.arrays);
      auto *__restrict rc = std::get<2>(chunk.arrays);

      const float rando =
          static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f;
      for (size_t i = 0; i < chunk.count; ++i) {

        if (psc[i].autoEmit && psc[i].elapsed > psc[i].interval) {
          SprayParticle &p = psc[i].particles[psc[i].currentParticle];
          psc[i].currentParticle =
              (psc[i].currentParticle + 1) % psc[i].maxNumberOfParticles;

          const float maxAngleDeg = psc[i].randAngleFactor; // 0..360
          const float maxAngleRad = glm::radians(maxAngleDeg);

          // random value in [-0.5, 0.5]
          // random angle in range
          const float angleRange = rando * maxAngleRad;
          const glm::mat2 rotation =
              glm::mat2(cos(angleRange), -sin(angleRange), //
                        sin(angleRange), cos(angleRange));
          const glm::vec2 normal = rotation * glm::normalize(rc->m_rotation);

          p = {.offset = tc->m_position,
               .normal = normal,
               .startTime = currentTime,
               .alive = true};
          psc[i].elapsed = 0;
        }

        renderer.beginSprayParticle(psc[i]);
        for (size_t j = 0; j < psc[i].particles.size(); j++) {
          SprayParticle &pa = psc[i].particles[j];
          if (pa.alive)
            renderer.drawSprayParticle(pa);
          // Remove dead particles
          if (currentTime - pa.startTime >= psc[i].lifeTime) {
            pa.alive = false;
          }
        }
      }
    }
  }
}

} // namespace Systems
} // namespace pain
