#include "AllBuffers.h"
#include <GL/glew.h>

namespace pain
{

// ======================================================================== //
// IndexBuffer
// ======================================================================== //

IndexBuffer::IndexBuffer(unsigned int *indexes, unsigned int count)
    : m_count(count)
{

  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indexes,
               GL_STATIC_DRAW);
}
void IndexBuffer::bind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}
void IndexBuffer::unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

// ======================================================================== //
// VertexBuffer
// ======================================================================== //

VertexBuffer::VertexBuffer(float *vertices, unsigned int size)
{
  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_bufferId); }
void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

} // namespace pain
