#pragma once
#include "pch.gch"

#include "BufferLayout.h"
#include "Core.h"

namespace pain
{

class VertexBuffer
{
public:
  VertexBuffer(float *vertices, uint32_t size);
  ~VertexBuffer() {}

  void bind() const;
  void unbind() const;

  inline const BufferLayout &getLayout() const { return m_layout; }
  inline void setLayout(const BufferLayout &layout) { m_layout = layout; }

private:
  uint32_t m_bufferId;
  BufferLayout m_layout;
};

class IndexBuffer
{
public:
  IndexBuffer(uint32_t *indexes, uint32_t count);
  ~IndexBuffer() {}

  void bind() const;
  void unbind() const;

  uint32_t getCount() const { return m_count; };

private:
  uint32_t m_bufferId;
  uint32_t m_count;
};
} // namespace pain
