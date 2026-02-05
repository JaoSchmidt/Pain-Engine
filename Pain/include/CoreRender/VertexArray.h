/** VertexArray.h */
#pragma once
#include "pch.h"

#include "Core.h"
#include "CoreRender/Buffers.h"

namespace pain
{

/**
 * GPU vertex array object that binds a vertex buffer and an index buffer.
 *
 * **Lifetime:** The referenced buffers must outlive this object.
 */
class VertexArray
{
public:
  NONCOPYABLE(VertexArray);
  VertexArray(VertexArray &&o);

  // ============================================================= //
  // **Creation**
  // ============================================================= //

  /** Creates a vertex array from an existing vertex and index buffer. */
  static std::optional<VertexArray>
  createVertexArray(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer);
  static std::optional<VertexArray>
  createVertexArray(VertexBuffer &vertexBuffer,
                    VertexBuffer &instanceVertexBuffer,
                    IndexBuffer &indexBuffer);

  ~VertexArray();

  // ============================================================= //
  // **Binding**
  // ============================================================= //

  /** Binds this vertex array. */
  void bind() const;

  /** Unbinds the currently bound vertex array. */
  void unbind() const;

  // ============================================================= //
  // **Access**
  // ============================================================= //

  /** Returns the associated index buffer. */
  const IndexBuffer &getIndexBuffer() const
  {
    return std::as_const(m_indexBuffer);
  }

  /** Returns the backend vertex array handle. */
  uint32_t getId() const { return m_rendererId; }

private:
  static void addVertexBuffer(const VertexBuffer &vertexBuffer,
                              uint32_t rendererId, uint32_t &index);
  static void setIndexBuffer(const IndexBuffer &indexBuffer,
                             uint32_t rendererId);

  VertexArray(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer,
              uint32_t rendererId);
  // NOTE: in theory, even thought opengl can create multiple vertex arrays, it
  // might be worth to just use 1 vertex arrays and ALL vertexBuffer. Remember
  // what cherno said about using multiple vertex arrays: "no need"
  VertexBuffer &m_vertexBuffer;
  IndexBuffer &m_indexBuffer;
  uint32_t m_rendererId;
};

} // namespace pain
