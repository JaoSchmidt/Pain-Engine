// VertexArrayBackend.h
#pragma once

#include "CoreRender/Buffers.h"
#include <cstdint>

namespace pain::backend
{

uint32_t createVertexArray();
void destroyVertexArray(uint32_t id);
void bindVertexArray(uint32_t id);
void unbindVertexArray();
void addVertexBuffer(const VertexBuffer &vertexBuffer, uint32_t rendererId);
void setIndexBuffer(const IndexBuffer &indexBuffer, uint32_t rendererId);
} // namespace pain::backend
