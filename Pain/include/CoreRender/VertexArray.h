#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/AllBuffers.h"

namespace pain
{
class VertexArray
{
public:
  NONCOPYABLE(VertexArray);
  VertexArray(VertexArray &&o);
  VertexArray &operator=(VertexArray &&o);
  ~VertexArray();
  VertexArray();
  void bind() const;
  void unbind() const;
  void addVertexBuffer(VertexBuffer *vertexBuffer);
  void setIndexBuffer(IndexBuffer *indexBuffer);
  // const std::vector<std::shared_ptr<VertexBuffer>> &getVertexBuffers() const
  // {
  //   return m_vertexBuffer;
  // };
  const IndexBuffer &getIndexBuffer() const
  {
    return std::as_const(*m_indexBuffer);
  }

private:
  VertexBuffer *m_vertexBuffer = nullptr;
  IndexBuffer *m_indexBuffer = nullptr;
  uint32_t m_rendererId;
};

} // namespace pain
