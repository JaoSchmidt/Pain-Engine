// BuffersBackend.h
#pragma once

#include <cstdint>

namespace pain::backend
{

struct VertexBufferCreateInfo {
  uint32_t size;
  const void *data;
  bool dynamic;
};

uint32_t createVertexBuffer(const VertexBufferCreateInfo &info);
void destroyVertexBuffer(uint32_t id);
void bindVertexBuffer(uint32_t id);
void unbindVertexBuffer();
void setVertexBufferData(uint32_t id, const void *data, uint32_t size);

uint32_t createIndexBuffer(const uint32_t *indices, uint32_t count);
void destroyIndexBuffer(uint32_t id);
void bindIndexBuffer(uint32_t id);
void unbindIndexBuffer();

} // namespace pain::backend
