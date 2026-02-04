/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file ParticleSprayComponent.h
 * @brief ECS component for lightweight particle emission and storage.
 *
 * Provides a simple particle container and emitter configuration used by
 * particle simulation and rendering systems. This component does not perform
 * updates by itself — it only stores particle state and emission parameters.
 */

#pragma once

#include "Assets/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Misc.h"
#include "ECS/Components/ComponentManager.h"
#include "pch.h"

#include "glm/fwd.hpp"
#include <cstdlib>
#include <vector>

namespace pain
{

/**
 * @brief Runtime state for a single particle instance.
 *
 * Stores position, motion direction, lifetime tracking and basic animation
 * parameters. Particles are recycled inside a fixed-size buffer owned by
 * ParticleSprayComponent.
 */
struct GridParticle {
  glm::vec2 m_direction = glm::vec2(1.f); /**< Emission direction vector. */
  Color color = StrongPink;
};

/**
 * @brief ECS component representing a particle emitter and particle buffer.
 *
 * Stores configuration parameters controlling particle behavior and maintains
 * a fixed-size circular buffer of Particle instances.
 *
 * Emission overwrites older particles once the buffer capacity is reached.
 * The component itself does not update particle motion or lifetime — this is
 * handled by external systems.
 */
struct GridParticleComponent {
  using tag = tag::GridParticle;

  uint64_t m_lifeTime =
      DeltaTime::oneSecond();    /**< Particle lifetime in time units. */
  float m_sizeChangeSpeed = 0.f; /**< Size growth / shrink speed. */
  float m_randSizeFactor = 0.f;  /**< Random size variance factor. */
  glm::vec2 m_emiterPositon =
      glm::vec2(0.f); /**< World-space emitter position. */

  std::vector<GridParticle> m_particles =
      {}; /**< Circular buffer of particles. */

  unsigned m_maxNumberOfParticles = 100; /**< Maximum particle capacity. */
  unsigned m_numberOfParticles = 0;      /**< Current emission index. */

  // ------------------------------------------------------------
  // Factory functions
  // ------------------------------------------------------------

  /**
   * @brief Creates a particle spray with default configuration.
   *
   * Allocates the particle buffer using the default maximum capacity.
   */
  static GridParticleComponent create()
  {
    GridParticleComponent c{};
    c.m_particles.resize(c.m_maxNumberOfParticles);
    return c;
  }

  /**
   * @brief Creates a particle spray with custom configuration.
   *
   * @param velocity Base particle velocity.
   * @param lifeTime Lifetime of each particle.
   * @param maxNumberOfParticles Maximum buffer capacity.
   * @param color Base particle color.
   */
  static GridParticleComponent create(float velocity, uint64_t lifeTime,
                                      unsigned maxNumberOfParticles,
                                      const glm::vec4 &color = glm::vec4(1.f))
  {
    GridParticleComponent c{.m_lifeTime = lifeTime,
                            .m_maxNumberOfParticles = maxNumberOfParticles};

    c.m_particles.resize(maxNumberOfParticles);

    return c;
  }

  // ------------------------------------------------------------
  // Emission
  // ------------------------------------------------------------

  /**
   * @brief Emits a single particle into the buffer.
   *
   * The particle is written into the current buffer index and overwrites
   * older particles once the buffer capacity is exceeded (ring-buffer
   * behavior).
   *
   * @param currentTime Current simulation time.
   * @param basePosition Emission position offset.
   * @param baseNormal Emission direction vector.
   * @param rotationSpeed Optional angular rotation speed.
   */
  void emitParticle(const uint64_t currentTime, const glm::vec2 &basePosition,
                    const glm::vec2 &baseNormal, float rotationSpeed = 0.f)
  {
    GridParticle &particle = m_particles[m_numberOfParticles];
    m_numberOfParticles = (m_numberOfParticles + 1) % m_maxNumberOfParticles;
  }

  // ------------------------------------------------------------
  // Maintenance
  // ------------------------------------------------------------

  /**
   * @brief Marks all particles as inactive.
   *
   * Does not resize or deallocate the particle buffer.
   */
  void clear() {}
};

} // namespace pain
