#include "CoreRender/VertexArray.h"
#include "CoreRender/Buffers.h"
#include "VertexArrayBackend.h"

namespace pain
{

std::optional<VertexArray>
VertexArray::createVertexArray(VertexBuffer &vertexBuffer,
                               IndexBuffer &indexBuffer)
{
  uint32_t rendererId = backend::createVertexArray();

  addVertexBuffer(vertexBuffer, rendererId);
  setIndexBuffer(indexBuffer, rendererId);
  return VertexArray(vertexBuffer, indexBuffer, rendererId);
}
VertexArray::VertexArray(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer,
                         uint32_t rendererId)
    : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer),
      m_rendererId(rendererId) {};
VertexArray::VertexArray(VertexArray &&o)
    : m_vertexBuffer(o.m_vertexBuffer), m_indexBuffer(o.m_indexBuffer),
      m_rendererId(o.m_rendererId)
{
  o.m_rendererId = 0;
}
VertexArray &VertexArray::operator=(VertexArray &&o)
{
  if (this != &o) {
    m_vertexBuffer = std::move(o.m_vertexBuffer);
    m_indexBuffer = std::move(o.m_indexBuffer);
    m_rendererId = o.m_rendererId;
    o.m_rendererId = 0;
  }
  return *this;
}
VertexArray::~VertexArray()
{
  if (m_rendererId)
    backend::destroyVertexArray(m_rendererId);
  // buffers will be deleted later from here, which is the correct order
}
void VertexArray::bind() const { backend::bindVertexArray(m_rendererId); }
void VertexArray::unbind() const { backend::unbindVertexArray(); }
void VertexArray::addVertexBuffer(const VertexBuffer &vertexBuffer,
                                  uint32_t rendererId)
{
  backend::addVertexBuffer(vertexBuffer, rendererId);
}
void VertexArray::setIndexBuffer(const IndexBuffer &indexBuffer,
                                 uint32_t rendererId)
{
  backend::setIndexBuffer(indexBuffer, rendererId);
}
} // namespace pain
