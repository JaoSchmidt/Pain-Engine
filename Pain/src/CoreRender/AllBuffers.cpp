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
std::optional<IndexBuffer> IndexBuffer::createIndexBuffer(uint32_t *indexes,
                                                          uint32_t count)
{
  uint32_t bufferId = 0;
  glGenBuffers(1, &bufferId);
  if (bufferId == 0) {
    PLOG_W("Failed to generate vertex buffer");
    return std::nullopt;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indexes,
               GL_STATIC_DRAW);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    PLOG_W("OpenGL error while creating vertex buffer: 0x{:X}", err);
    glDeleteBuffers(1, &bufferId);
    return std::nullopt;
  }
  return IndexBuffer(bufferId, count);
}

void IndexBuffer::bind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}
void IndexBuffer::unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

// ======================================================================== //
// VertexBuffer
// ======================================================================== //

void VertexBuffer::setData(const void *data, uint32_t size)
{
  bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}
void VertexBuffer::bind() const
{
  // PLOG_I("Binded buffer {}", m_bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
}
void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

std::optional<VertexBuffer>
VertexBuffer::createVertexBuffer(float *vertices, uint32_t size,
                                 const BufferLayout &&layout)
{
  uint32_t bufferId = 0;
  glGenBuffers(1, &bufferId);
  if (bufferId == 0) {
    PLOG_W("Failed to generate vertex buffer");
    return std::nullopt;
  }
  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    PLOG_W("OpenGL error while creating vertex buffer: 0x{:X}", err);
    glDeleteBuffers(1, &bufferId);
    return std::nullopt;
  }
  return VertexBuffer(bufferId, std::move(layout));
}
std::optional<VertexBuffer>
VertexBuffer::createVertexBuffer(uint32_t size, const BufferLayout &&layout)
{
  uint32_t bufferId;
  glGenBuffers(1, &bufferId);
  if (bufferId == 0) {
    PLOG_W("Failed to generate vertex buffer");
    return std::nullopt;
  }
  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    PLOG_W("OpenGL error while creating vertex buffer: 0x{:X}", err);
    glDeleteBuffers(1, &bufferId);
    return std::nullopt;
  }
  return VertexBuffer(bufferId, std::move(layout));
}
VertexBuffer::VertexBuffer(uint32_t bufferId, const BufferLayout &&layout)
    : m_bufferId(bufferId), m_layout(std::move(layout)) {};

} // namespace pain
