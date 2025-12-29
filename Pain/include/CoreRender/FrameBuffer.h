#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
namespace pain
{

struct FrameBufferCreationInfo {
  bool swapChainTarget = false;
  int32_t width = 800;
  int32_t height = 600;
  uint32_t samples = 1;
  // equivallent to ask "do you want to render to the screen?"

  uint32_t colorAttachmentTextureId = 0;
  uint32_t depthAttachmentTextureId = 0;
  uint32_t bufferId = 0;
};

class FrameBuffer
{
public:
  static std::optional<FrameBuffer>
  createFrameBuffer(const FrameBufferCreationInfo &info);
  void resizeFrameBuffer(int32_t width, int32_t height);
  void bind();
  void unbind();

  FrameBuffer(const FrameBufferCreationInfo &spec) : m(spec) {};
  MOVABLE(FrameBuffer);
  NONCOPYABLE(FrameBuffer);
  const FrameBufferCreationInfo &getSpecification() const { return m; }
  const uint32_t &getColorAttachmentRendererId() const
  {
    return m.colorAttachmentTextureId;
  }

  uint32_t getWidth() const { return static_cast<uint32_t>(m.width); }
  float getWidthf() const { return static_cast<float>(m.width); }
  int32_t getWidthi() const { return static_cast<int32_t>(m.width); }

  uint32_t getHeight() const { return static_cast<uint32_t>(m.height); }
  float getHeightf() const { return static_cast<float>(m.height); }
  int32_t getHeighti() const { return static_cast<int32_t>(m.height); }

private:
  FrameBufferCreationInfo m;
};
} // namespace pain
