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
 * Stores motion direction, lifetime tracking and basic animation
 * parameters. Particles are recycled inside a fixed-size buffer owned by
 * ParticleSprayComponent.
 */
struct SprayParticle {
  glm::vec2 offset = {0.f, 0.f};
  glm::vec2 normal = {0.f, 0.f}; /**< Emission direction vector. */
  DeltaTime startTime = 0;       /**< Time when the particle was spawned. */
  bool alive = false; /**< Whether the particle is currently active. */
};

/**
 * @brief Safer way to initialize particle spray component
 */
struct ParticleSprayInitArgs {
  float rotationSpeed =
      1.f; /**< Spray Particle Batch: Angular rotation speed. */
  DeltaTime interval =
      DeltaTime::oneSecond() / 2; ///< elapsed limit, 0 means no emisions
  float velocity = 1.f;           ///< Base particle velocity.
  DeltaTime lifeTime = DeltaTime::oneSecond();
  float sizeChangeSpeed = 1.f;  ///< Size growth / shrink speed.
  float randSizeFactor = 1.f;   ///< Random size variance factor.
  float randAngleFactor = 30.f; ///< Angle of emittion variance factor
  bool autoEmit = true; ///< Whether particles automatically spwan or not
  Color color = Colors::Orange; ///< Particle base color.
  unsigned capacity;
};

/**
 * @brief ECS component representing a particle emitter and particle buffer.
 *
 * Stores configuration parameters controlling particle behavior and maintains
 * a fixed-size circular buffer `particles` of instances.
 *
 * Emission overwrites older particles once the buffer capacity is reached.
 * The component itself does not update particle motion or lifetime — this is
 * handled by external systems.
 */
struct ParticleSprayComponent {
  using tag = tag::ParticleSpray;

  DeltaTime elapsed = 0; /// current time between emitions
  DeltaTime interval =
      DeltaTime::oneSecond() / 2; /// elapsed limit, 0 means no emisions
  float velocity = 1.f;           ///< Base particle velocity.
  DeltaTime lifeTime =
      DeltaTime::oneSecond();  ///< Particle lifetime in time units.
  float sizeChangeSpeed = 1.f; ///< Size growth / shrink speed.
  float randSizeFactor = 1.f;  ///< Random size variance factor.
  float randAngleFactor =
      30.f; ///< Random initial angle variance factor, from 0 to 360 degrees
  float rotationSpeed = 1.f;    ///< Random rotation speed factor.
  Color color = Colors::Orange; ///< Particle base color.
  bool autoEmit = false;        /// Whether particles automatically spwan or not

  std::vector<SprayParticle> particles =
      {}; /**< Circular buffer of particles. */

  unsigned maxNumberOfParticles = 0; /**< Maximum particle capacity. */
  unsigned currentParticle = 0;      /**< Current emission index. */

  /// Set auto emit particles.
  void setAutoEmit(bool emitAutomatically)
  {
    autoEmit = emitAutomatically;
    if (maxNumberOfParticles == 0) {
      maxNumberOfParticles = 1;
    }
    if (particles.size() == 0) {
      particles.push_back(SprayParticle{});
    }
  }
  // ------------------------------------------------------------
  // Factory functions
  // ------------------------------------------------------------

  /**
   * @brief Creates a particle spray using ParticleSprayInitArgs
   *
   * PreAllocates the particle buffer using the default maximum capacity.
   */
  static ParticleSprayComponent create(ParticleSprayInitArgs args)
  {
    ParticleSprayComponent c;
    c.interval = args.interval;
    c.velocity = args.velocity;
    c.lifeTime = args.lifeTime;
    c.sizeChangeSpeed = args.sizeChangeSpeed;
    c.randSizeFactor = args.sizeChangeSpeed;
    c.autoEmit = args.autoEmit;
    c.maxNumberOfParticles = args.capacity;
    c.particles.resize(c.maxNumberOfParticles);
    c.randAngleFactor = args.randAngleFactor;
    if (args.capacity == 0) {
      c.autoEmit = false;
    } else {
      c.particles.push_back(SprayParticle{.alive = false});
    }
    return c;
  }

  // ------------------------------------------------------------
  // Maintenance
  // ------------------------------------------------------------

  /**
   * @brief Marks all particles as inactive.
   *
   * Does not resize or deallocate the particle buffer.
   */
  void clear()
  {
    for (auto &p : particles) {
      p.alive = false;
    }
  }
};

} // namespace pain
