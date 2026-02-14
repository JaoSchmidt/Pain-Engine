/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file LuaScriptSys.h
 * @brief System responsible for executing Lua script callbacks.
 *
 * This system iterates over entities containing LuaScriptComponent and invokes
 * their registered Lua callbacks during the engine update, event, and render
 * phases.
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
struct LuaScriptComponent;

namespace Systems
{

/**
 * @brief Executes Lua script callbacks attached to entities.
 *
 * LuaScript is an ECS system that processes entities containing
 * LuaScriptComponent and dispatches Lua callbacks for:
 *  - Frame updates
 *  - SDL events
 *  - Rendering
 *
 * The system queries all LuaScriptComponent instances and safely executes any
 * bound Lua functions (on_update, on_event, on_render), reporting runtime
 * errors to the logging system.
 *
 * @note The inheritance from IOnUpdate, IOnEvent, and IOnRender is REQUIRED
 * for the engine to register and invoke the corresponding callbacks.
 * Removing one of these interfaces will prevent that callback from being
 * executed entirely, even if the function exists in this class.
 *
 * @note This system declares a Tags list to ensure at compile time that
 * LuaScriptComponent is registered in the component manager before the system
 * can be inserted into a scene.
 *
 * @see System
 * @see IOnUpdate
 * @see IOnEvent
 * @see IOnRender
 * @see LuaScriptComponent
 */
struct LuaScript : public System<WorldComponents>,
                   IOnUpdate,
                   IOnEvent,
                   IOnRender {
  /**
   * @brief Component tags required by this system.
   *
   * The system operates on entities that contain LuaScriptComponent.
   * This tag list is validated at compile time when inserting the system
   * into a scene.
   */
  using Tags = TypeList<LuaScriptComponent>;

  /** @brief Inherit base System constructors. */
  using System::System;

  /**
   * @brief Executes Lua update callbacks for all script components.
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
   * @brief Dispatches SDL events to Lua script callbacks.
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
   * @brief Executes Lua rendering callbacks.
   *
   * Called during the render phase after visibility and renderer state
   * have been resolved.
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
