#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/BufferLayout.h"

namespace pain
{

class VertexBuffer
{
public:
  static std::optional<VertexBuffer>
  createVertexBuffer(uint32_t size, const BufferLayout &&layout);
  static std::optional<VertexBuffer>
  createVertexBuffer(float *vertices, uint32_t size,
                     const BufferLayout &&layout);
  ~VertexBuffer();

  void bind() const;
  void unbind() const;

  inline const BufferLayout &getLayout() const { return m_layout; }
  void setData(const void *data, uint32_t size);

  // I'm assuing this will only be own by some Renderer
  MOVABLE(VertexBuffer)
  NONCOPYABLE(VertexBuffer)
private:
  VertexBuffer(uint32_t bufferId, const BufferLayout &&layout);
  uint32_t m_bufferId;
  BufferLayout m_layout;
};

class IndexBuffer
{
public:
  IndexBuffer(uint32_t *indexes, uint32_t count);
  ~IndexBuffer() {}
  static std::optional<IndexBuffer> createIndexBuffer(uint32_t *indexes,
                                                      uint32_t count);
  void bind() const;
  void unbind() const;
  const uint32_t getCount() const { return m_count; };

  // I'm assuing this will only be own by some Renderer
  NONCOPYABLE(IndexBuffer)
  MOVABLE(IndexBuffer)
private:
  IndexBuffer(uint32_t bufferId, uint32_t count);
  uint32_t m_bufferId;
  uint32_t m_count;
};
} // namespace pain
