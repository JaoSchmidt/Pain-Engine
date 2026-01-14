#include "TextureBackend.h"

#ifdef PAIN_RENDERER_OPENGL

#include "platform/OpenGL/OpenGLDebugger.h"
#include <glad/gl.h>

#include "CoreFiles/LogWrapper.h"

namespace pain::backend
{

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static constexpr unsigned getInternalFormat(ImageFormat format)
{
  constexpr unsigned types[] = {
      GL_R8,      // None = 0,
      GL_R8,      // R8,
      GL_RGB8,    // RGB8,
      GL_RGBA8,   // RGBA8,
      GL_RGBA32F, // RGBA32F
  };
  static_assert(static_cast<unsigned>(ImageFormat::RGBA32F) <
                    sizeof(types) / sizeof(types[0]),
                "Missing entry in types array");
  return types[static_cast<uint32_t>(format)];
}

static constexpr unsigned getGLDataFormat(ImageFormat format)
{
  constexpr unsigned types[] = {
      GL_RG,   // None = 0,
      GL_RG,   // R8,
      GL_RGB,  // RGB8,
      GL_RGBA, // RGBA8,
      GL_RGBA, // RGBA32F
  };
  static_assert(static_cast<unsigned>(ImageFormat::RGBA32F) <
                    sizeof(types) / sizeof(types[0]),
                "Missing entry in types array");
  return types[static_cast<uint32_t>(format)];
}
// ------------------------------------------------------------
// Texture creation
// ------------------------------------------------------------

uint32_t createTexture(const TextureCreateInfo &info)
{
  uint32_t id = 0;
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
  if (!id) {
    PLOG_W("Texture \"{}\" could not be created!", info.debugName);
    return 0;
  }

  GLenum internal = getInternalFormat(info.format);

  glTextureStorage2D(id, 1, internal, static_cast<int32_t>(info.width),
                     static_cast<int32_t>(info.height));
  glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

  PLOG_I("Texture created id = {}, name = {}", id, info.debugName);
  return id;
}
/// Sets a texture given a specific texture image path
uint32_t createTextureFromFile(const TextureFromFileInfo &info)
{
  GLenum dataFormat;
  GLenum internalFormat;

  if (info.channels == 3) {
    dataFormat = GL_RGB;
    internalFormat = GL_RGB8;
  } else if (info.channels == 4) {
    dataFormat = GL_RGBA;
    internalFormat = GL_RGBA8;
  } else {
    PLOG_W("Texture \"{}\" not created, unsupported channel count {}",
           info.path, info.channels);
    return 0;
  }

  uint32_t id = 0;
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
  if (!id) {
    PLOG_W("Texture \"{}\" could not be created!", info.path);
    return 0;
  }

  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  if (info.clampToEdge) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int32_t>(internalFormat),
               info.width, info.height, 0, dataFormat, GL_UNSIGNED_BYTE,
               info.pixels);

  P_OPENGL_CHECK("Failed glTexImage2D inside Texture");

  PLOG_I("Texture created id = {}, path = {}", id, info.path);
  return id;
}

// ------------------------------------------------------------
// Usage
// ------------------------------------------------------------

void destroyTexture(uint32_t id) { glDeleteTextures(1, &id); }

void bindTexture(uint32_t id) { glBindTexture(GL_TEXTURE_2D, id); }

// bind texture unit i.e. has a slot in a sample array. In theory this is
// depreciated because I'm caching the slots inside the Texture Object (inside
// pain engine)
void bindTextureToSlot(uint32_t id, uint32_t slot)
{
  glBindTextureUnit(slot, id);
}

void setTextureData(uint32_t id, uint32_t width, uint32_t height,
                    ImageFormat dataFormat, const void *data, uint32_t size)
{
  uint32_t bytesPerPixel = getGLDataFormat(dataFormat) == GL_RGBA ? 4 : 3;
  P_ASSERT_W(size == width * height * bytesPerPixel,
             "Data must be entire texture!");
  glTextureSubImage2D(id, 0, 0, 0, static_cast<int32_t>(width),
                      static_cast<int32_t>(height), getGLDataFormat(dataFormat),
                      GL_UNSIGNED_BYTE, data);
}

} // namespace pain::backend

#endif
