/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file NativeScriptSys.h
 * @brief System responsible for executing native C++ script callbacks.
 *
 * This system iterates over entities containing NativeScriptComponent and
 * invokes their registered C++ callbacks during the engine update, event, and
 * render phases.
 *
 * Native scripts are bound directly in C++ and represent gameplay
 * logic, controllers, or engine-driven behaviors that require tight
 * integration with the runtime.
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *  - IOnEvent   → enables onEvent() callbacks.
 *  - IOnRender  → enables onRender() callbacks.
 *
 */

#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
struct Renderer2d;
struct NativeScriptComponent;

namespace Systems
{

/**
 * @brief Executes native C++ script callbacks attached to entities.
 *
 * NativeScript is an ECS system that processes entities containing
 * NativeScriptComponent and dispatches registered callbacks for:
 *  - Per-frame updates
 *  - SDL events
 *  - Rendering
 *
 * The system queries all NativeScriptComponent instances and invokes the
 * bound function pointers if a script instance exists and the callback is
 * registered.
 *
 * @note This system declares a Tags list to enforce at compile time that
 * NativeScriptComponent is registered in the component manager before the
 * system can be added to a scene.
 *
 * @see System
 * @see IOnUpdate
 * @see IOnEvent
 * @see IOnRender
 * @see NativeScriptComponent
 */
struct NativeScript : public System<WorldComponents>,
                      IOnUpdate,
                      IOnEvent,
                      IOnRender {
  /**
   * @brief Component tags required by this system.
   *
   * The system operates exclusively on entities that contain
   * NativeScriptComponent. This tag list is validated at compile time when
   * inserting the system into a scene.
   */
  using Tags = TypeList<NativeScriptComponent>;

  /** @brief Inherit base System constructors. */
  using System::System;

  /**
   * @brief Executes native script update callbacks for all script components.
   *
   * Called once per frame during the update phase.
   *
   * @param deltaTime Frame delta time.
   *
   * @note This method will only be called if the system inherits from
   * IOnUpdate.
   */
  void onUpdate(DeltaTime deltaTime) override;

  /**
   * @brief Dispatches SDL events to native script callbacks.
   *
   * Invoked whenever an SDL event is propagated through the engine.
   *
   * @param e Incoming SDL event.
   *
   * @note This method will only be called if the system inherits from
   * IOnEvent.
   */
  void onEvent(const SDL_Event &e) override;

  /**
   * @brief Executes native script rendering callbacks.
   *
   * Called during the render phase after renderer state has been resolved.
   *
   * @param renderer Active 2D renderer.
   * @param isMinimized Indicates whether the application window is minimized.
   * @param currentTime Current frame time or delta time reference.
   *
   * @note This method will only be called if the system inherits from
   * IOnRender.
   */
  void onRender(Renderers &renderer, bool isMinimized,
                DeltaTime currentTime) override;
};

} // namespace Systems
} // namespace pain
