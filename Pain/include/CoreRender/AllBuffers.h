#pragma once

namespace pain
{

class VertexBuffer
{
public:
  VertexBuffer(float *vertices, unsigned int size);
  ~VertexBuffer() {}

  void Bind() const;
  void Unbind() const;

private:
  unsigned int m_bufferId;
};

class IndexBuffer
{
public:
  IndexBuffer(unsigned int *indeces, unsigned int count);
  ~IndexBuffer() {}

  void Bind() const;
  void Unbind() const;

  unsigned int GetCount() const { return m_count; };

private:
  unsigned int m_bufferId;
  unsigned int m_count;
};

} // namespace pain
