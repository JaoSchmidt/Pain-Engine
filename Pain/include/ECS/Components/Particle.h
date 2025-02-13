#pragma once

#include "CoreFiles/LogWrapper.h"
#include "pch.h"

#include "glm/fwd.hpp"
#include <cstdlib>
#include <vector>

#include "ECS/Registry/Macros.h"

namespace pain
{
int constexpr bitMaskUnit(int bit) { return 1 << bit; }
struct Particle {
  glm::vec2 m_position;
  glm::vec2 m_offset;
  glm::vec2 m_normal;
  float m_startTime;
  float m_rotationSpeed;
  bool m_alive = false;
};

struct ParticleSprayComponent {
  static constexpr int componentID = bitMaskUnit(4);
  float m_velocity = 1.f;
  float m_lifeTime = 1.f;
  float m_sizeChangeSpeed = 0.f;
  float m_randSizeFactor = 0.f;
  glm::vec4 m_color = glm::vec4(1.f);
  glm::vec2 m_emiterPosition = glm::vec2(0.f);
  std::vector<Particle> m_particles = {};
  int m_maxNumberOfParticles = 100;
  int m_numberOfParticles = 0;

  ParticleSprayComponent()
  {
    for (int i = 0; i < m_maxNumberOfParticles; i++) {
      Particle particle;
      m_particles.push_back(particle);
    }
  };
  ParticleSprayComponent(float velocity, float lifeTime,
                         unsigned int maxNumberOfParticles,
                         const glm::vec4 &color = glm::vec4(1.f))
      : m_velocity(velocity), m_lifeTime(lifeTime), m_color(color),
        m_maxNumberOfParticles(maxNumberOfParticles)
  {
    for (int i = 0; i < m_maxNumberOfParticles; i++) {
      Particle particle;
      m_particles.push_back(particle);
      const float rando = (float)rand() / RAND_MAX;
      particle.m_rotationSpeed = rando;
      particle.m_alive = false;
    }
  }

  void emitParticle(const float currentTime, const glm::vec2 &basePosition,
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
