/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file TrailCmp.h
 * @brief ECS component for lightweight particle emission and storage.
 *
 * Provides a simple particle container and emitter configuration used by
 * particle simulation and rendering systems. This component does not perform
 * updates by itself — it only stores particle state and emission parameters.
 */

#pragma once
#include "Assets/DeltaTime.h"
#include "CoreRender/Renderer/Misc.h"
#include "ECS/Components/ComponentManager.h"

namespace pain
{
/**
 * @brief Safer way to initialize TrailComponent
 */
struct TrailComponentInitArgs {
  unsigned capacity = 256;         ///< Maximum number of stored trail points
  float minDistance = 0.01f;       ///< Minimum distance before adding new point
  bool autoEmit = true;            ///< Whether trail records automatically
  Color color = Colors::FullWhite; ///< Trail color
  DeltaTime lifeTime = DeltaTime::oneSecond(); ///< Lifetime of each point
};

struct TrailComponent {
  using tag = tag::ParticleTrail;

  std::vector<glm::vec2> points;

  unsigned capacity = 256;
  float minDistance = 0.01f; // avoid too many points
  bool autoEmit = true;      ///< Whether particles automatically spwan or not
  Color color = Colors::FullWhite;
  DeltaTime lifeTime = DeltaTime::oneSecond();
};

/**
 * @brief Creates a TrailComponent using TrailComponentInitArgs
 *
 * Preallocates the point buffer using the specified capacity.
 */
static TrailComponent create(const TrailComponentInitArgs &args)
{
  TrailComponent c;

  c.capacity = args.capacity;
  c.minDistance = args.minDistance;
  c.autoEmit = args.autoEmit;
  c.color = args.color;
  c.lifeTime = args.lifeTime;

  // Reserve capacity to avoid reallocations during runtime
  c.points.reserve(c.capacity);

  return c;
}

} // namespace pain
