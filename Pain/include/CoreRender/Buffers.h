#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/BufferLayout.h"

namespace pain
{

class VertexBuffer
{
public:
  static std::optional<VertexBuffer> createVertexBuffer(uint32_t size,
                                                        BufferLayout &&layout);
  static std::optional<VertexBuffer>
  createStaticVertexBuffer(float *vertices, uint32_t size,
                           BufferLayout &&layout);
  VertexBuffer(VertexBuffer &&o);
  VertexBuffer &operator=(VertexBuffer &&o);
  NONCOPYABLE(VertexBuffer)
  ~VertexBuffer();

  void bind() const;
  void unbind() const;

  uint32_t getId() const { return m_bufferId; }
  const BufferLayout &getLayout() const { return m_layout; }
  void setData(const void *data, uint32_t size);

  // I'm assuing this will only be own by some Renderer

private:
  VertexBuffer(uint32_t bufferId, BufferLayout &&layout);
  uint32_t m_bufferId;
  BufferLayout m_layout;
};

class IndexBuffer
{
public:
  static std::optional<IndexBuffer> createIndexBuffer(uint32_t *indexes,
                                                      uint32_t count);
  IndexBuffer(IndexBuffer &&o);
  IndexBuffer &operator=(IndexBuffer &&o);
  NONCOPYABLE(IndexBuffer);
  ~IndexBuffer();
  void bind() const;
  void unbind() const;
  uint32_t getCount() const { return m_count; };

private:
  IndexBuffer(uint32_t bufferId, uint32_t count);
  uint32_t m_bufferId;
  uint32_t m_count;
};
} // namespace pain
