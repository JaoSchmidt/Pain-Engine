module;

#include "Core.h"
export module pain.AllBuffers;
import <cstdint>;
import pain.BufferLayout;

export namespace pain
{

class VertexBuffer
{
public:
  VertexBuffer(uint32_t size);
  VertexBuffer(float *vertices, uint32_t size);
  ~VertexBuffer() {}

  void bind() const;
  void unbind() const;

  inline const BufferLayout &getLayout() const { return m_layout; }
  inline void setLayout(const BufferLayout &layout) { m_layout = layout; }
  void setData(const void *data, uint32_t size);

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
