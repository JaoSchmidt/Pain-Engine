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

class RenderPipeline
{
public:
  static RenderPipeline create(reg::EventDispatcher &eventDispatcher);

  static RenderPipeline create(const FrameBufferCreationInfo &info,
                               reg::EventDispatcher &eventDispatcher);

  void onWindowResized(const SDL_Event &event, Renderer2d &renderer,
                       Scene &scene);
  void pipeline(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime,
                Scene &worldScene, UIScene &uiScene);

  FrameBuffer m_frameBuffer;
  reg::EventDispatcher &m_eventDispatcher;

  void subscribeToViewportChange(Scene &scene);

private:
  explicit RenderPipeline(FrameBuffer frameBuffer,
                          reg::EventDispatcher &eventDispatcher);
  // explicit RenderPipeline(FrameBuffer frameBuffer);
};

} // namespace pain
