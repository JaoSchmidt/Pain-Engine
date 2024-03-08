#pragma once

#include "BufferLayout.h"
#include <cstdint>

namespace pain
{

class VertexBuffer
{
public:
  VertexBuffer(float *vertices, uint32_t size);
  ~VertexBuffer() {}

  void Bind() const;
  void Unbind() const;

  inline const BufferLayout &GetLayout() const { return m_layout; }
  inline void SetLayout(const BufferLayout &layout) { m_layout = layout; }

private:
  uint32_t m_bufferId;
  BufferLayout m_layout;
};

class IndexBuffer
{
public:
  IndexBuffer(uint32_t *indexes, uint32_t count);
  ~IndexBuffer() {}

  void Bind() const;
  void Unbind() const;

  uint32_t GetCount() const { return m_count; };

private:
  uint32_t m_bufferId;
  uint32_t m_count;
};
} // namespace pain
