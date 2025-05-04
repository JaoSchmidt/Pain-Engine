module;
#include "Core.h"
#include "pch.h"

export module pain.VertexArray;

import pain.AllBuffers;

export namespace pain
{
class VertexArray
{
public:
  // NONCOPYABLE(VertexArray)
  ~VertexArray();
  VertexArray();
  void bind() const;
  void unbind() const;
  void addVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer);
  void setIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer);
  const std::vector<std::shared_ptr<VertexBuffer>> &getVertexBuffers() const
  {
    return m_vertexBuffer;
  };
  const std::shared_ptr<IndexBuffer> &getIndexBuffer() const
  {
    return m_indexBuffer;
  }

private:
  std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffer;
  std::shared_ptr<IndexBuffer> m_indexBuffer;
  uint32_t m_rendererId;
};

} // namespace pain
