/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RenderPipeline.h
#pragma once

#include "CoreRender/FrameBuffer.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Scene.h"

namespace pain
{

/**
 * @class RenderPipeline
 * @brief Manages the high-level rendering flow of the engine.
 *
 * The RenderPipeline owns the main framebuffer and coordinates rendering
 * of the World scene and UI scene. It also reacts to window and viewport
 * resize events and updates camera projections accordingly.
 */
class RenderPipeline
{
public:
  /**
   * @brief Creates a render pipeline using the *default* framebuffer
   * configuration.
   *
   * This overload creates a framebuffer configured as a swap-chain target.
   *
   * @param eventDispatcher Event dispatcher used for viewport subscriptions.
   * @return Constructed RenderPipeline instance.
   */
  static RenderPipeline create(reg::EventDispatcher &eventDispatcher);

  /**
   * @brief Creates a render pipeline using a custom framebuffer configuration.
   *
   * @param info Framebuffer creation parameters.
   * @param eventDispatcher Event dispatcher used for viewport subscriptions.
   * @return Constructed RenderPipeline instance.
   */
  static RenderPipeline create(const FrameBufferCreationInfo &info,
                               reg::EventDispatcher &eventDispatcher);

  /**
   * @brief Handles window resize events and updates cameras and viewport.
   *
   * This function propagates resize events to all active orthographic cameras
   * in the provided scene and resizes the framebuffer when required.
   *
   * @param event SDL window event.
   * @param renderer Active renderer instance.
   * @param scene Scene containing camera components.
   */
  void onWindowResized(const SDL_Event &event, Renderer2d &renderer,
                       Scene &scene);

  /**
   * @brief Executes a full render pass for the current frame.
   *
   * The pipeline performs:
   * - Framebuffer binding and clearing
   * - World scene rendering
   * - Framebuffer unbinding
   * - UI scene rendering
   *
   * @param renderer Active renderer instance.
   * @param isMinimized Indicates whether the window is minimized.
   * @param currentTime Current frame delta time.
   * @param worldScene Scene containing world entities and systems.
   * @param uiScene Scene containing UI entities and systems.
   */
  void pipeline(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime,
                Scene &worldScene, UIScene &uiScene);

  /** @brief Framebuffer owned by the render pipeline. */
  FrameBuffer m_frameBuffer;

  /**  @brief Event dispatcher used for resize and viewport events. */
  reg::EventDispatcher &m_eventDispatcher;

  /**
   * @brief Subscribes to viewport change events for camera updates.
   *
   * When an ImGui viewport change event occurs, all orthographic cameras
   * in the scene will update their projection and framebuffer size.
   *
   * @param scene Scene containing camera components.
   */
  void subscribeToViewportChange(Scene &scene);

private:
  /**
   * @brief Constructs a render pipeline from an existing framebuffer.
   *
   * Intended to be used internally by the factory methods.
   *
   * @param frameBuffer Framebuffer instance.
   * @param eventDispatcher Event dispatcher reference.
   */
  explicit RenderPipeline(FrameBuffer frameBuffer,
                          reg::EventDispatcher &eventDispatcher);
};

} // namespace pain
