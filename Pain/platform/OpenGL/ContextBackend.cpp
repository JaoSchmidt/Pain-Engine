/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// ContextBackend.cpp
#include "ContextBackend.h"

#ifdef PAIN_RENDERER_OPENGL

#include "CoreFiles/LogWrapper.h"
#include "OpenGLDebugger.h"
#include <glad/gl.h>

namespace pain::backend
{
namespace
{
int s_fragmentUnits;
}

void Init()
{
  int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  if (version == 0) {
    PLOG_E("Error: Failed to initialize glad");
    exit(1);
  }
  PLOG_T("GL version: {}",
         std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION))));
  // PLOG_T("Default relative path is: {}",
  //        std::filesystem::current_path().string());

  int versionMajor;
  int versionMinor;
  glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
  glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
  P_ASSERT(versionMajor >= 4 && versionMinor >= 3,
           "OpenGL version must be above 4.3, current version is {}.{}",
           versionMajor, versionMinor);

  GLint maxTextureUnits;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
  PLOG_T("GPU Texture Mapping Units: {}", maxTextureUnits);
  s_fragmentUnits = maxTextureUnits;
#ifndef NDEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(Debug::glErrorHandler, 0);
#endif
}
void InitRenderer()
{
  // =============================================================== //
  // Create Renderer
  // =============================================================== //

  // NOTE: This enable 3d and can be changed later in case we need some camera
  // mechanic
  // Also, to enable GL_DEPTH, you must also enable those:
  // GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
  glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LEQUAL);
  // glDepthMask(GL_TRUE);

  // allow transparency
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // HACK: allow textures with 3 channels to align properly, e.g. font textures.
  // No idea why it works tho, perhaps I will find a proper doc later
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}
void setViewPort(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}
void setClearColor(const glm::vec4 &color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}
void clear()
{
  if (glIsEnabled(GL_DEPTH_TEST))
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  else
    glClear(GL_COLOR_BUFFER_BIT);
}
void drawInstanced(uint32_t indiceCount, uint32_t instanceCount)
{
  glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<int32_t>(indiceCount),
                        static_cast<int32_t>(instanceCount));
  glBindTexture(GL_TEXTURE_2D, 0);
}
void drawIndexed(const VertexArray &vertexArray, uint32_t indexCount)
{
  uint32_t count =
      indexCount ? indexCount : vertexArray.getIndexBuffer().getCount();
  glDrawElements(GL_TRIANGLES, static_cast<int32_t>(count), GL_UNSIGNED_INT,
                 nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned getTMU() { return static_cast<unsigned>(s_fragmentUnits); }
int getTMUi() { return s_fragmentUnits; }

} // namespace pain::backend

#endif
