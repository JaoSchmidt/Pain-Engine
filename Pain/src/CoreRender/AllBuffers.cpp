#include "CoreRender/AllBuffers.h"
#include "CoreFiles/LogWrapper.h"

namespace pain
{

// ======================================================================== //
// IndexBuffer
// ======================================================================== //

IndexBuffer::IndexBuffer(uint32_t *indexes, uint32_t count) : m_count(count)
{

  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indexes,
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

VertexBuffer::VertexBuffer(uint32_t size)
{
  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(float *vertices, uint32_t size)
{
  glGenBuffers(1, &m_bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_bufferId); }
void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VertexBuffer::setData(const void *data, uint32_t size)
{
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

} // namespace pain
