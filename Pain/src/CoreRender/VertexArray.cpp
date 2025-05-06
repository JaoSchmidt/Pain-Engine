module;
#include "glad/gl.h" // Including OpenGL headers in global fragment

module pain.VertexArray;
import pain.AllBuffers;
import <memory>;

namespace pain
{

VertexArray::VertexArray() { glCreateVertexArrays(1, &m_rendererId); }
VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_rendererId); }
void VertexArray::bind() const { glBindVertexArray(m_rendererId); }
void VertexArray::unbind() const { glBindVertexArray(0); }
void VertexArray::addVertexBuffer(
    const std::shared_ptr<VertexBuffer> &vertexBuffer)
{
  assert(vertexBuffer->getLayout().GetElements().size() &&
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
  m_vertexBuffer.push_back(vertexBuffer);
}
void VertexArray::setIndexBuffer(
    const std::shared_ptr<IndexBuffer> &indexBuffer)
{
  glBindVertexArray(m_rendererId);
  indexBuffer->bind();
  m_indexBuffer = indexBuffer;
}
} // namespace pain
