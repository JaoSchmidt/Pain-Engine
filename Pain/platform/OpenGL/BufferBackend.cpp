/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "BuffersBackend.h"

#ifdef PAIN_RENDERER_OPENGL

#include "CoreFiles/LogWrapper.h"
#include <glad/gl.h>

namespace pain::backend
{

uint32_t createVertexBuffer(const VertexBufferCreateInfo &info)
{
  uint32_t id = 0;
  glGenBuffers(1, &id);
  if (!id) {
    PLOG_W("Failed to generate vertex buffer");
    return 0;
  }

  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, info.size, info.data,
               info.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    PLOG_W("OpenGL error while creating vertex buffer: 0x{:X}", err);
    glDeleteBuffers(1, &id);
    return 0;
  }
  return id;
}

void destroyVertexBuffer(uint32_t id) { glDeleteBuffers(1, &id); }

void bindVertexBuffer(uint32_t id) { glBindBuffer(GL_ARRAY_BUFFER, id); }

void unbindVertexBuffer() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void setVertexBufferData(uint32_t id, const void *data, uint32_t size)
{
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

// ---------------- IndexBuffer ----------------

uint32_t createIndexBuffer(const uint32_t *indices, uint32_t count)
{
  uint32_t id = 0;
  glGenBuffers(1, &id);
  if (!id) {
    PLOG_W("Failed to generate vertex buffer");
    return 0;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices,
               GL_STATIC_DRAW);
  return id;
}

void destroyIndexBuffer(uint32_t id) { glDeleteBuffers(1, &id); }

void bindIndexBuffer(uint32_t id) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }

void unbindIndexBuffer() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

} // namespace pain::backend

#endif
