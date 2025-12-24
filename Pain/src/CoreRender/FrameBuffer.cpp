#include "CoreRender/FrameBuffer.h"

#include "platform/FrameBufferBackend.h"

namespace pain
{

std::optional<FrameBuffer>
FrameBuffer::createFrameBuffer(const FrameBufferCreationInfo &info)
{
  FrameBufferCreationInfo fb = info;
  if (info.swapChainTarget)
    return FrameBuffer{fb};

  uint32_t frameBufferId = backend::createFrameBuffer(fb);

  if (frameBufferId) {
    fb.bufferId = frameBufferId;
    return FrameBuffer{fb};
  } else
    return std::nullopt;
}

void FrameBuffer::bind() { pain::backend::bindFrameBuffer(m.bufferId); }
void FrameBuffer::unbind() { pain::backend::unbindFrameBuffer(); }

} // namespace pain
