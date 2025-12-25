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
  static RenderPipeline create();
  static RenderPipeline create(const FrameBufferCreationInfo &info);

  void onWindowResized(const SDL_Event &event, Renderer2d &renderer,
                       Scene &scene);
  void pipeline(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime,
                Scene &worldScene);

  FrameBuffer m_frameBuffer;

private:
  explicit RenderPipeline(FrameBuffer frameBuffer);
};

} // namespace pain
