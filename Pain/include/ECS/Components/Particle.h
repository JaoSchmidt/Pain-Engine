#pragma once

#include "pch.h"

#include "glm/fwd.hpp"
#include <queue>
#include <vector>

namespace pain
{
struct Particle {
  glm::vec2 m_position;
  glm::vec2 m_offset;
  glm::vec2 m_normal;
  float m_startTime;
  float m_rotationSpeed;
  bool m_alive = false;
};

struct ParticleSprayComponent {
  float m_velocity = 1.f;
  float m_lifeTime = 10.f;
  glm::vec4 m_color = glm::vec4(1.f);
  glm::vec2 m_emiterPosition = glm::vec2(0.f);
  std::vector<Particle> m_particles = {};
  int MaxNumberOfParticles = 100;
  int m_numberOfParticles = 0;

  ParticleSprayComponent() = default;
  ParticleSprayComponent(float velocity, float lifeTime, const glm::vec4 &color)
      : m_velocity(velocity), m_lifeTime(lifeTime), m_color(color)
  {
    for (int i = 0; i < MaxNumberOfParticles; i++) {
      Particle particle;
      m_particles.push_back(particle);
    }
  }

  void emitParticle(const float currentTime, const glm::vec2 &basePosition,
                    const glm::vec2 &baseNormal)
  {
    Particle &particle = m_particles[m_numberOfParticles];
    m_numberOfParticles++;

    particle.m_position = {0.f, 0.f};    // Move the particle
    particle.m_startTime -= currentTime; // Decrease life time
    particle.m_alive = true;
  }
};
} // namespace pain
