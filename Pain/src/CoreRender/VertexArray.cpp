#include "CoreRender/VertexArray.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/AllBuffers.h"

namespace pain
{

VertexArray::VertexArray() { glCreateVertexArrays(1, &m_rendererId); }
VertexArray::VertexArray(VertexArray &&o)
    : m_vertexBuffer(o.m_vertexBuffer), m_indexBuffer(o.m_indexBuffer),
      m_rendererId(o.m_rendererId)
{
  o.m_rendererId = 0;
}
VertexArray &VertexArray::operator=(VertexArray &&o)
{
  if (this != &o) {
    m_vertexBuffer = o.m_vertexBuffer;
    m_indexBuffer = o.m_indexBuffer;
    m_rendererId = o.m_rendererId;
    o.m_rendererId = 0;
  }
  return *this;
}
VertexArray::~VertexArray()
{
  if (m_rendererId != 0)
    glDeleteVertexArrays(1, &m_rendererId);
}
void VertexArray::bind() const { glBindVertexArray(m_rendererId); }
void VertexArray::unbind() const { glBindVertexArray(0); }
void VertexArray::addVertexBuffer(VertexBuffer *vertexBuffer)
{
  P_ASSERT(
      vertexBuffer->getLayout().GetElements().size() > 0,
      "VertexArray.h: Can't add a vertexBuffer that doesn't have a layout");
  glBindVertexArray(m_rendererId);
  vertexBuffer->bind();

  uint32_t index = 0;
  const auto &layout = vertexBuffer->getLayout();
  for (const auto &element : layout) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(                       //
        index,                                   //
        element.getComponentCount(),             //
        element.getComponentGLType(),            //
        element.normalized ? GL_TRUE : GL_FALSE, //
        layout.GetStride(),                      //
        // same as `(const void *)element.offset`, but won't generate warning
        reinterpret_cast<const void *>(static_cast<uintptr_t>(element.offset)));
    index++;
  }
  m_vertexBuffer = vertexBuffer;
}
void VertexArray::setIndexBuffer(IndexBuffer *indexBuffer)
{
  glBindVertexArray(m_rendererId);
  indexBuffer->bind();
  m_indexBuffer = indexBuffer;
}
} // namespace pain
