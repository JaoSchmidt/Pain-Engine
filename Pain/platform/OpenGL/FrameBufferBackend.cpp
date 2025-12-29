// FrameBuffer.cpp
#include "FrameBufferBackend.h"

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
  uint32_t frameBufferId;
  glCreateFramebuffers(1, &frameBufferId);
  if (!frameBufferId) {
    PLOG_W("frameBufferId was not able to be created");
    return frameBufferId;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
  glCreateTextures(GL_TEXTURE_2D, 1, &spec.colorAttachmentTextureId);
  glBindTexture(GL_TEXTURE_2D, spec.colorAttachmentTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  P_OPENGL_CHECK("Failed glTexImage2D");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         spec.colorAttachmentTextureId, 0);

  if (glIsEnabled(GL_DEPTH_TEST)) {

    glCreateTextures(GL_TEXTURE_2D, 1, &spec.depthAttachmentTextureId);
    glBindTexture(GL_TEXTURE_2D, spec.depthAttachmentTextureId);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, spec.width,
                   spec.height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, spec.width, spec.height,
                 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    P_OPENGL_CHECK("Failed glTexImage2D");
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           GL_TEXTURE_2D, spec.depthAttachmentTextureId, 0);
  }
  P_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
           "Framebuffer is incomplete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return frameBufferId;
}
void resizeFrameBuffer(const FrameBufferCreationInfo &spec)
{
  glBindTexture(GL_TEXTURE_2D, spec.colorAttachmentTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  if (glIsEnabled(GL_DEPTH_TEST)) {
    glBindTexture(GL_TEXTURE_2D, spec.depthAttachmentTextureId);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, spec.width,
                   spec.height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec.width, spec.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  }
}

void deleteFrameBuffer(uint32_t rendererId)
{
  glDeleteFramebuffers(1, &rendererId);
}

} // namespace pain::backend
#endif
