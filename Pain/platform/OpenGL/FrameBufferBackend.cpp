/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// FrameBuffer.cpp
#include "platform/FrameBufferBackend.h"

#ifdef PAIN_RENDERER_OPENGL

#include "CoreFiles/LogWrapper.h"
#include "platform/OpenGL/OpenGLDebugger.h"
#include <glad/gl.h>

namespace pain::backend
{

void bindFrameBuffer(uint32_t rendererId)
{
  glBindFramebuffer(GL_FRAMEBUFFER, rendererId);
}
void unbindFrameBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
uint32_t createFrameBuffer(FrameBufferCreationInfo &spec)
{
  glCreateFramebuffers(1, &spec.bufferId);
  glBindFramebuffer(GL_FRAMEBUFFER, spec.bufferId);

  // ----- COLOR ATTACHMENT -----
  glCreateTextures(GL_TEXTURE_2D, 1, &spec.colorAttachmentTextureId);
  glBindTexture(GL_TEXTURE_2D, spec.colorAttachmentTextureId);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         spec.colorAttachmentTextureId, 0);

  // ----- DEPTH ATTACHMENT  -----
  glCreateTextures(GL_TEXTURE_2D, 1, &spec.depthAttachmentTextureId);
  glBindTexture(GL_TEXTURE_2D, spec.depthAttachmentTextureId);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, spec.width, spec.height,
               0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D, spec.depthAttachmentTextureId, 0);

  // Tell OpenGL which color attachments to draw to
  GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, buffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    PLOG_E("Framebuffer is incomplete!");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return spec.bufferId;
}
/** Resize width and height of the framebuffer */
void resizeFrameBuffer(const FrameBufferCreationInfo &spec)
{
  glBindTexture(GL_TEXTURE_2D, spec.colorAttachmentTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  P_ASSERT_W(spec.depthAttachmentTextureId, "Texture is invalid on framebuffer")
  glBindTexture(GL_TEXTURE_2D, spec.depthAttachmentTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, spec.width, spec.height,
               0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
}

void deleteFrameBuffer(uint32_t rendererId)
{
  glDeleteFramebuffers(1, &rendererId);
}

} // namespace pain::backend
#endif
