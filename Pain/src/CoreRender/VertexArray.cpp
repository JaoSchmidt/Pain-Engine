#include "CoreRender/VertexArray.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/AllBuffers.h"

namespace pain
{

std::optional<VertexArray>
VertexArray::createVertexArray(VertexBuffer &&vertexBuffer,
                               IndexBuffer &indexBuffer)
{
  uint32_t rendererId;
  glCreateVertexArrays(1, &rendererId);
  if (rendererId == 0) {
    PLOG_W("Failed to generate vertex buffer");
    return std::nullopt;
  }

  addVertexBuffer(vertexBuffer, rendererId);
  setIndexBuffer(indexBuffer, rendererId);
  return VertexArray(std::move(vertexBuffer), indexBuffer, rendererId);
}
VertexArray::VertexArray(VertexBuffer &&vertexBuffer, IndexBuffer &indexBuffer,
                         uint32_t rendererId)
    : m_vertexBuffer(std::move(vertexBuffer)), m_indexBuffer(indexBuffer),
      m_rendererId(rendererId) {};
VertexArray::VertexArray(VertexArray &&o)
    : m_vertexBuffer(std::move(o.m_vertexBuffer)),
      m_indexBuffer(o.m_indexBuffer), m_rendererId(o.m_rendererId)
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
  if (m_rendererId != 0)
    glDeleteVertexArrays(1, &m_rendererId);
  // buffers will be deleted later from here, which is the correct order
}
void VertexArray::bind() const { glBindVertexArray(m_rendererId); }
void VertexArray::unbind() const { glBindVertexArray(0); }
void VertexArray::addVertexBuffer(const VertexBuffer &vertexBuffer,
                                  uint32_t rendererId)
{
  P_ASSERT(
      vertexBuffer.getLayout().GetElements().size() > 0,
      "VertexArray.h: Can't add a vertexBuffer that doesn't have a layout");
  glBindVertexArray(rendererId);
  vertexBuffer.bind();

  uint32_t index = 0;
  const auto &layout = vertexBuffer.getLayout();
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
}
void VertexArray::setIndexBuffer(const IndexBuffer &indexBuffer,
                                 uint32_t rendererId)
{
  glBindVertexArray(rendererId);
  indexBuffer.bind();
}
} // namespace pain
