#pragma once

#include "AllBuffers.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace pain
{
class VertexArray
{
public:
  ~VertexArray();
  VertexArray();
  void bind() const;
  void unbind() const;
  void addVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer);
  void setIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer);

private:
  std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffer;
  std::shared_ptr<IndexBuffer> m_indexBuffer;
  uint32_t m_rendererId;
};

} // namespace pain
