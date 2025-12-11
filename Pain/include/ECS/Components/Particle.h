#pragma once

#include "Assets/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
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
  float m_velocity = 1.f;
  uint64_t m_lifeTime = DeltaTime::oneSecond();
  float m_sizeChangeSpeed = 0.f;
  float m_randSizeFactor = 0.f;
  glm::vec4 m_color = glm::vec4(1.f);
  glm::vec2 m_emiterPosition = glm::vec2(0.f);
  std::vector<Particle> m_particles = {};
  unsigned m_maxNumberOfParticles = 100;
  unsigned m_numberOfParticles = 0;

  ParticleSprayComponent()
  {
    for (unsigned i = 0; i < m_maxNumberOfParticles; i++) {
      Particle particle;
      m_particles.push_back(particle);
    }
  };
  ParticleSprayComponent(float velocity, uint64_t lifeTime,
                         unsigned maxNumberOfParticles,
                         const glm::vec4 &color = glm::vec4(1.f))
      : m_velocity(velocity), m_lifeTime(lifeTime), m_color(color),
        m_maxNumberOfParticles(maxNumberOfParticles)
  {
    for (unsigned i = 0; i < m_maxNumberOfParticles; i++) {
      Particle particle;
      m_particles.push_back(particle);
      const float rando = (float)rand() / (float)RAND_MAX;
      particle.m_rotationSpeed = rando;
      particle.m_alive = false;
    }
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
