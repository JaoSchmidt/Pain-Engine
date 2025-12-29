// FrameBufferBackend.h
#pragma once

#include "CoreRender/FrameBuffer.h"
namespace pain::backend
{

void bindFrameBuffer(uint32_t rendererId);
void unbindFrameBuffer();
uint32_t createFrameBuffer(FrameBufferCreationInfo &spec);
void deleteFrameBuffer(uint32_t rendererId);
void resizeFrameBuffer(const FrameBufferCreationInfo &spec);

} // namespace pain::backend
