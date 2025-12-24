// RenderPipeline.h
#pragma once

#include <array>
#include <optional>

#include "CoreRender/FrameBuffer.h"
#include "CoreRender/Renderer/Renderer2d.h"

namespace pain
{

class RenderPipeline
{
public:
  static RenderPipeline create();
  static RenderPipeline create(const FrameBufferCreationInfo &info);

  void pipeline(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime,
                Scene &worldScene);

  FrameBuffer m_frameBuffer;

private:
  explicit RenderPipeline(FrameBuffer frameBuffer);
};

} // namespace pain
