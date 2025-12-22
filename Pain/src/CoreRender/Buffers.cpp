#include "CoreRender/Buffers.h"
#include "CoreFiles/LogWrapper.h"
#include "platform/BuffersBackend.h"
namespace pain
{

// ======================================================================== //
// IndexBuffer
// ======================================================================== //

std::optional<IndexBuffer> IndexBuffer::createIndexBuffer(uint32_t *indexes,
                                                          uint32_t count)
{
  if (!indexes || count == 0) {
    PLOG_W("IndexBuffer creation failed: invalid data");
    return std::nullopt;
  }

  uint32_t bufferId = backend::createIndexBuffer(indexes, count);
  if (bufferId == 0) {
    PLOG_W("Failed to create IndexBuffer backend resource");
    return std::nullopt;
  }

  return IndexBuffer(bufferId, count);
}

void IndexBuffer::bind() const { backend::bindIndexBuffer(m_bufferId); }

void IndexBuffer::unbind() const { backend::unbindIndexBuffer(); }

IndexBuffer::~IndexBuffer()
{
  if (m_bufferId)
    backend::destroyIndexBuffer(m_bufferId);
}

IndexBuffer::IndexBuffer(uint32_t bufferId, uint32_t count)
    : m_bufferId(bufferId), m_count(count) {};

IndexBuffer::IndexBuffer(IndexBuffer &&o)
    : m_bufferId(o.m_bufferId), m_count(o.m_count)
{
  o.m_bufferId = 0;
}
IndexBuffer &IndexBuffer::operator=(IndexBuffer &&o)
{
  if (this != &o) {
    if (m_bufferId != 0)
      backend::destroyIndexBuffer(m_bufferId);
    m_bufferId = o.m_bufferId;
    m_count = o.m_count;
    o.m_bufferId = 0;
  }
  return *this;
}

// ======================================================================== //
// VertexBuffer
// ======================================================================== //

VertexBuffer::VertexBuffer(VertexBuffer &&o)
    : m_bufferId(o.m_bufferId), m_layout(std::move(o.m_layout))
{
  o.m_bufferId = 0;
};
VertexBuffer &VertexBuffer::operator=(VertexBuffer &&o)
{
  if (this != &o) {
    m_bufferId = o.m_bufferId;
    m_layout = std::move(o.m_layout);
    o.m_bufferId = 0;
  }
  return *this;
}

VertexBuffer::VertexBuffer(uint32_t bufferId, BufferLayout &&layout)
    : m_bufferId(bufferId), m_layout(std::move(layout)) {};

std::optional<VertexBuffer>
VertexBuffer::createStaticVertexBuffer(float *vertices, uint32_t size,
                                       BufferLayout &&layout)
{
  backend::VertexBufferCreateInfo info{
      .size = size, .data = vertices, .dynamic = false};

  uint32_t id = backend::createVertexBuffer(info);
  if (!id) {
    PLOG_W("Failed to generate vertex buffer");
    return std::nullopt;
  }

  return VertexBuffer(id, std::move(layout));
}

std::optional<VertexBuffer>
VertexBuffer::createVertexBuffer(uint32_t size, BufferLayout &&layout)
{
  backend::VertexBufferCreateInfo info{
      .size = size, .data = nullptr, .dynamic = true};

  uint32_t id = backend::createVertexBuffer(info);
  if (!id) {
    PLOG_W("Failed to generate vertex buffer");
    return std::nullopt;
  }

  return VertexBuffer(id, std::move(layout));
}

void VertexBuffer::bind() const { backend::bindVertexBuffer(m_bufferId); }

void VertexBuffer::unbind() const { backend::unbindVertexBuffer(); }

void VertexBuffer::setData(const void *data, uint32_t size)
{
  backend::setVertexBufferData(m_bufferId, data, size);
}

VertexBuffer::~VertexBuffer()
{
  if (m_bufferId)
    backend::destroyVertexBuffer(m_bufferId);
}

} // namespace pain
