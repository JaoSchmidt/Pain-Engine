#include "AllBuffers.h"
#include <GL/glew.h>

namespace pain
{

// ======================================================================== //
// IndexBuffer
// ======================================================================== //

IndexBuffer::IndexBuffer(unsigned int *indices, unsigned int count)
    : m_count(count)
{

  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices,
               GL_STATIC_DRAW);
}
void IndexBuffer::Bind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}
void IndexBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

// ======================================================================== //
// VertexBuffer
// ======================================================================== //

VertexBuffer::VertexBuffer(float *vertices, unsigned int size)
{
  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_bufferId); }
void VertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

} // namespace pain
