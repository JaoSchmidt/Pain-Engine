#include "TextureBackend.h"

#ifdef PAIN_RENDERER_OPENGL

#include <SDL2/SDL_surface.h>
#include <SDL_image.h>
#include <cstring>
#include <glad/gl.h>
#include <memory>

#include "CoreFiles/LogWrapper.h"

namespace pain::backend
{

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static SDL_Surface *flipVertical(const SDL_Surface &surface)
{
  SDL_Surface *result = SDL_CreateRGBSurface(
      surface.flags, surface.w, surface.h, surface.format->BytesPerPixel * 8,
      surface.format->Rmask, surface.format->Gmask, surface.format->Bmask,
      surface.format->Amask);

  const size_t pitch = static_cast<unsigned long>(surface.pitch);
  const size_t pxlength = pitch * (static_cast<unsigned long>(surface.h - 1));

  auto *src = static_cast<unsigned char *>(surface.pixels) + pxlength;
  auto *dst = static_cast<unsigned char *>(result->pixels);

  for (int y = 0; y < surface.h; ++y) {
    std::memcpy(dst, src, pitch);
    src -= pitch;
    dst += pitch;
  }

  return result;
}

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

  glTextureStorage2D(id, 1, internal, info.width, info.height);
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
  // SDL_Surface *flipped = flipVertical(*surface);

  GLenum dataFormat, internalFormat;
  SDL_Surface *surface = info.surface;
  if (surface->format->BytesPerPixel == 3) {
    dataFormat = GL_RGB;
    internalFormat = GL_RGB8;
  } else if (surface->format->BytesPerPixel == 4) {
    dataFormat = GL_RGBA;
    internalFormat = GL_RGBA8;
  } else {
    PLOG_W(
        "Texture \"{}\" not created, unsupported number of bytes per pixel {}",
        info.path, surface->format->BytesPerPixel);
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

  // https://stackoverflow.com/questions/10568390/difference-between-uv-and-st-texture-coordinates
  if (info.clampToEdge) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->w, surface->h, 0,
               dataFormat, GL_UNSIGNED_BYTE, surface->pixels);

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
  glTextureSubImage2D(id, 0, 0, 0, width, height, getGLDataFormat(dataFormat),
                      GL_UNSIGNED_BYTE, data);
}

} // namespace pain::backend

#endif
