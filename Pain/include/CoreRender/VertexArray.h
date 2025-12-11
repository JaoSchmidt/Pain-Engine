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
  std::optional<VertexArray> static createVertexArray(
      VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer);

  ~VertexArray();
  void bind() const;
  void unbind() const;
  // const std::vector<std::shared_ptr<VertexBuffer>> &getVertexBuffers() const
  // {
  //   return m_vertexBuffer;
  // };
  const IndexBuffer &getIndexBuffer() const
  {
    return std::as_const(m_indexBuffer);
  }
  uint32_t getId() const { return m_rendererId; }

private:
  static void addVertexBuffer(const VertexBuffer &vertexBuffer,
                              uint32_t rendererId);
  static void setIndexBuffer(const IndexBuffer &indexBuffer,
                             uint32_t rendererId);
  VertexArray(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer,
              uint32_t rendererId);
  // NOTE: in theory, even thought opengl can create multiple vertex arrays, it
  // might be worth to just use 1 vertex arrays and ALL vertexBuffer. Remember
  // what cherno said about using multiple vertex arrays: "no need"
  VertexBuffer &m_vertexBuffer;
  IndexBuffer &m_indexBuffer;
  uint32_t m_rendererId;
};

} // namespace pain
