/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file KinematicsSys.h
 * @brief System responsible for updating basic kinematic motion and rotation.
 *
 * This system updates:
 *  - RotationComponent: Recomputes the direction vector from the rotation
 * angle.
 *  - Transform2dComponent + Movement2dComponent: Integrates velocity to update
 *    entity positions.
 *
 * It provides a lightweight kinematic step suitable for simple physics,
 * prototyping, and gameplay movement logic.
 *
 * @note As with any systems in the engine, system headers follow the naming
 * convention and end with the suffix "Sys.h".
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *
 * If the interface is removed, the engine will no longer invoke the associated
 * callback.
 */

#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"
#include "Physics/MovementComponent.h"
#include "Physics/Particles/SprayCmp.h"

namespace pain
{
namespace Systems
{

/**
 * @brief Performs per-frame kinematic integration and rotation updates.
 *
 * Kinematics iterates over relevant component sets and performs:
 *  - Rotation vector updates based on RotationComponent::m_rotationAngle.
 *  - Position integration using velocity and frame delta time.
 *
 * The system operates entirely during the update phase and does not participate
 * in rendering or event processing.
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *
 * @note The Tags list is validated at compile time to ensure that all required
 * components are registered in the component manager before this system can be
 * added to a scene.
 *
 * @see Transform2dComponent
 * @see Movement2dComponent
 * @see RotationComponent
 * @see System
 * @see IOnUpdate
 */
struct ParticleSys : public System<WorldComponents>, IOnUpdate, IOnRender {
  /**
   * @brief Component tags required by this system.
   *
   * Declares that this system operates on entities containing:
   *  - Transform2dComponent
   *  - Movement2dComponent
   *
   * RotationComponent is queried independently during update.
   */
  using Tags = TypeList<Transform2dComponent, Movement2dComponent,
                        ParticleSprayComponent>;

  /** @brief Inherit base System constructors. */
  using System<WorldComponents>::System;

  /** @brief Prevent default construction. */
  ParticleSys() = delete;

  /**
   * @brief Performs kinematic updates for the current frame.
   *
   * Updates rotation vectors and integrates velocity into position using the
   * provided delta time.
   *
   * @param deltaTime Frame delta time.
   *
   * @note This method is invoked only because the system inherits from
   * IOnUpdate.
   */
  void onUpdate(DeltaTime deltaTime) override;
  void onRender(pain::Renderer2d &renderer, bool isMinimized,
                DeltaTime deltaTime) override;
};

} // namespace Systems
} // namespace pain
