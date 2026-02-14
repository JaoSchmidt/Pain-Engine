/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// ContextBackend.cpp
#include "VertexArrayBackend.h"
#include "CoreRender/BufferLayout.h"

#ifdef PAIN_RENDERER_OPENGL

#include "CoreFiles/LogWrapper.h"
#include "OpenGLDebugger.h"
#include <glad/gl.h>

namespace pain::backend
{
constexpr GLint getComponentCount(ShaderDataType type)
{
  constexpr uint32_t counts[] = {
      0,     // None
      1,     // Float
      2,     // Float2
      3,     // Float3
      4,     // Float4
      3 * 3, // Mat3
      4 * 4, // Mat4
      1,     // Int
      2,     // Int2
      3,     // Int3
      4,     // Int4
      1,     // Bool
      1,     // UBtye
      2,     // UBtye2
      3,     // UBtye3
      4,     // UBtye4
  };
  static_assert(static_cast<uint32_t>(ShaderDataType::Bool) <
                    sizeof(counts) / sizeof(counts[0]),
                "Missing entry in counts array");

  return static_cast<GLint>(counts[static_cast<uint32_t>(type)]);
}
constexpr GLenum getComponentGLType(ShaderDataType type)
{
  constexpr GLenum types[] = {
      GL_FLOAT,         // NONE
      GL_FLOAT,         // Float
      GL_FLOAT,         // Float2
      GL_FLOAT,         // Float3
      GL_FLOAT,         // Float4
      GL_FLOAT,         // Mat3
      GL_FLOAT,         // Mat4
      GL_INT,           // Int
      GL_INT,           // Int2
      GL_INT,           // Int3
      GL_INT,           // Int4
      GL_BOOL,          // Bool
      GL_UNSIGNED_BYTE, // UByte
      GL_UNSIGNED_BYTE, // UByte
      GL_UNSIGNED_BYTE, // UByte
      GL_UNSIGNED_BYTE, // UByte
  };

  static_assert(static_cast<GLenum>(ShaderDataType::Bool) <
                    sizeof(types) / sizeof(types[0]),
                "Missing entry in types array");

  return types[static_cast<uint32_t>(type)];
}

uint32_t createVertexArray()
{
  uint32_t rendererId;
  glCreateVertexArrays(1, &rendererId);
  if (rendererId == 0) {
    PLOG_W("Failed to generate vertex buffer");
    return 0;
  }
  return rendererId;
}

void destroyVertexArray(uint32_t id)
{
  glDeleteVertexArrays(1, &id);
  // buffers will be deleted later from here, which is the correct order
}

void bindVertexArray(uint32_t id) { glBindVertexArray(id); }
void unbindVertexArray() { glBindVertexArray(0); }

void addVertexBuffer(const VertexBuffer &vertexBuffer, uint32_t rendererId,
                     uint32_t &index)
{
  P_ASSERT(
      vertexBuffer.getLayout().getElements().size() > 0,
      "VertexArray.h: Can't add a vertexBuffer that doesn't have a layout");
  bindVertexArray(rendererId);
  vertexBuffer.bind();

  const auto &layout = vertexBuffer.getLayout();
  for (const BufferElement &element : layout) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(                        //
        index,                                    //
        getComponentCount(element.type),          //
        getComponentGLType(element.type),         //
        element.normalized ? GL_TRUE : GL_FALSE,  //
        static_cast<GLsizei>(layout.getStride()), //
        // same as `(const void *)element.offset`, but won't generate warning
        reinterpret_cast<const void *>(static_cast<uintptr_t>(element.offset)));
    if (element.instanced)
      glVertexAttribDivisor(index, 1);
    index++;
  }
}
void setIndexBuffer(const IndexBuffer &indexBuffer, uint32_t rendererId)
{
  bindVertexArray(rendererId);
  indexBuffer.bind(); // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferId);
}
} // namespace pain::backend

#endif
