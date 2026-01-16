/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


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

void FrameBuffer::resizeFrameBuffer(int32_t width, int32_t height)
{
  m.width = width;
  m.height = height;
  pain::backend::resizeFrameBuffer(m);
}
void FrameBuffer::bind() { pain::backend::bindFrameBuffer(m.bufferId); }
void FrameBuffer::unbind() { pain::backend::unbindFrameBuffer(); }

} // namespace pain
