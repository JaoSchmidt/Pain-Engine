/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file CameraSys.h
 * @brief Event-driven camera system for handling window and projection updates.
 *
 * CameraSys listens for engine and window events and updates orthographic
 * camera components accordingly. Its primary responsibility is to react to
 * window resize events and recompute camera projection parameters such as
 * aspect ratio, projection bounds, and resolution.
 *
 * This system currently operates only on entities containing an OrthoCamera
 * component and performs no per-frame updates or rendering logic.
 *
 * @warning This system is currently marked as work-in-progress (WIP) and should
 * not be relied upon for production usage.
 *
 * @note As with any systems in the engine, system headers follow the naming
 * convention and end with the suffix "Sys.h".
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnEvent   → enables onEvent() callbacks.
 *
 * If the interface is removed, the engine will no longer invoke the associated
 * callback.
 */
#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace cmp
{
struct OrthoCamera;
}

namespace pain ::Systems
{
/**
 * @brief Event-driven system for updating orthographic camera state.
 *
 * CameraSys reacts to SDL window events and propagates resolution and
 * projection changes into all registered OrthoCamera components. This allows
 * cameras to automatically adjust their aspect ratio and projection bounds when
 * the window is resized.
 *
 * The system does not perform any spatial updates, physics integration, or
 * rendering logic.
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnEvent   → enables onEvent() callbacks.
 *
 * @note The Tags list is validated at compile time to ensure that all required
 * components are registered in the component manager before this system can be
 * added to a scene.
 *
 * @see cmp::OrthoCamera
 * @see System
 * @see IOnEvent
 */
struct CameraSys : public System<WorldComponents>, IOnEvent {
  /**
   * @brief Component tags required by this system.
   *
   * Declares that this system operates on entities containing:
   *  - cmp::OrthoCamera
   */
  using Tags = TypeList<::cmp::OrthoCamera>;

  /** @brief Inherit base System constructors. */
  using System::System;

  /**
   * @brief Handles incoming SDL events and updates camera state when required.
   *
   * Responds primarily to window resize events and recomputes camera projection
   * parameters such as aspect ratio and resolution for each active camera
   * component.
   *
   * @param event Incoming SDL event.
   *
   * @note This method is invoked only because the system inherits from
   * IOnEvent.
   */
  void onEvent(const SDL_Event &event) override;
};
} // namespace pain::Systems
