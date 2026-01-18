/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "Assets/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "pch.h"

#include "glm/fwd.hpp"
#include <cstdlib>
#include <vector>

namespace pain
{
struct Particle {
  glm::vec2 m_position;
  glm::vec2 m_offset;
  glm::vec2 m_normal;
  DeltaTime m_startTime;
  float m_rotationSpeed;
  bool m_alive = false;
};

struct ParticleSprayComponent {
  using tag = tag::ParticleSpray;
  float m_velocity = 1.f;
  uint64_t m_lifeTime = DeltaTime::oneSecond();
  float m_sizeChangeSpeed = 0.f;
  float m_randSizeFactor = 0.f;
  glm::vec4 m_color = glm::vec4(1.f);
  glm::vec2 m_emiterPosition = glm::vec2(0.f);
  std::vector<Particle> m_particles = {};
  unsigned m_maxNumberOfParticles = 100;
  unsigned m_numberOfParticles = 0;

  static ParticleSprayComponent create()
  {
    ParticleSprayComponent c{};
    c.m_particles.resize(c.m_maxNumberOfParticles);
    return c;
  }

  static ParticleSprayComponent create(float velocity, uint64_t lifeTime,
                                       unsigned maxNumberOfParticles,
                                       const glm::vec4 &color = glm::vec4(1.f))
  {
    ParticleSprayComponent c{.m_velocity = velocity,
                             .m_lifeTime = lifeTime,
                             .m_color = color,
                             .m_maxNumberOfParticles = maxNumberOfParticles};

    c.m_particles.resize(maxNumberOfParticles);

    for (auto &particle : c.m_particles) {
      particle.m_rotationSpeed =
          static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      particle.m_alive = false;
    }

    return c;
  }

  void emitParticle(const uint64_t currentTime, const glm::vec2 &basePosition,
                    const glm::vec2 &baseNormal, float rotationSpeed = 0.f)
  {
    Particle &particle = m_particles[m_numberOfParticles];
    m_numberOfParticles = (m_numberOfParticles + 1) % m_maxNumberOfParticles;

    particle.m_position = {0.f, 0.f};         // Move the particle
    particle.m_normal = baseNormal;           // Move the particle
    particle.m_offset = basePosition;         // Move the particle
    particle.m_startTime = currentTime;       // Decrease life time
    particle.m_rotationSpeed = rotationSpeed; // Speed of rotation
    particle.m_alive = true;
  }

  void clear()
  {
    for (auto &p : m_particles) {
      p.m_alive = false;
    }
  }
};
} // namespace pain
