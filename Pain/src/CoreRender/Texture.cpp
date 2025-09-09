#include "CoreRender/Texture.h"
#include "Core.h"

#include "CoreRender/Renderer/Renderer2d.h"
#include "glad/gl.h"
#include <SDL2/SDL_surface.h>
#include <SDL_image.h>
#include <cstdint>

namespace pain
{
SDL_Surface flipVertical(const SDL_Surface &surface)
{
  SDL_Surface result = *SDL_CreateRGBSurface(
      surface.flags, surface.w, surface.h, surface.format->BytesPerPixel * 8,
      surface.format->Rmask, surface.format->Gmask, surface.format->Bmask,
      surface.format->Amask);
  const auto pitch = surface.pitch;
  const auto pxlength = pitch * (surface.h - 1);
  auto pixels = static_cast<unsigned char *>(surface.pixels) + pxlength;
  auto rpixels = static_cast<unsigned char *>(result.pixels);
  for (auto line = 0; line < surface.h; ++line) {
    memcpy(rpixels, pixels, pitch);
    pixels -= pitch;
    rpixels += pitch;
  }
  return result;
}

bool Texture::operator==(const Texture &other) const
{
  return m_rendererId == other.m_rendererId;
}
/*
 * Sets a dump texture
 */
std::optional<Texture> Texture::createTexture(const char *name, uint32_t width,
                                              uint32_t height,
                                              ImageFormat format)
{
  uint32_t internalFormat = getInternalFormat(format);
  uint32_t dataFormat = getGLDataFormat(format);

  uint32_t rendererId;
  glCreateTextures(GL_TEXTURE_2D, 1, &rendererId);
  glBindTexture(GL_TEXTURE_2D, rendererId);
  glTextureStorage2D(rendererId, 1, internalFormat, width, height);

  glTextureParameteri(rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(rendererId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTextureParameteri(rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);
  return Texture(name, width, height, dataFormat, internalFormat, rendererId);
}
/*
 * Sets a texture given a specific texture image path
 */
std::optional<Texture> Texture::createTexture(const char *path,
                                              bool gl_clamp_to_edge)
{
  std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface = {
      IMG_Load(path), SDL_FreeSurface};
  P_ASSERT_W(surface != nullptr, "Texture path \"{}\" was not found!", path);
  if (surface == nullptr)
    return {};

  uint32_t rendererId;
  glCreateTextures(GL_TEXTURE_2D, 1, &rendererId);
  glBindTexture(GL_TEXTURE_2D, rendererId);

  uint32_t dataFormat, internalFormat;
  if (surface->format->BytesPerPixel == 3) {
    dataFormat = GL_RGB;
    internalFormat = GL_RGB8;
  } else if (surface->format->BytesPerPixel == 4) {
    dataFormat = GL_RGBA;
    internalFormat = GL_RGBA8;
  } else {
    P_ASSERT(false, "Unsupported number of bytes per pixel {}",
             surface->format->BytesPerPixel);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // https://stackoverflow.com/questions/10568390/difference-between-uv-and-st-texture-coordinates
  if (gl_clamp_to_edge) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->w, surface->h, 0,
               dataFormat, GL_UNSIGNED_BYTE, surface->pixels);
  return Texture(path, surface->w, surface->h, dataFormat, internalFormat,
                 rendererId);
}

void Texture::setData(const void *data, uint32_t size)
{
  uint32_t bytesPerPixel = m_dataFormat == GL_RGBA ? 4 : 3;
  P_ASSERT_W(size == m_width * m_height * bytesPerPixel,
             "Data must be entire texture!");
  glTextureSubImage2D(m_rendererId, 0, 0, 0, m_width, m_height, m_dataFormat,
                      GL_UNSIGNED_BYTE, data);
}

// bind texture unit i.e. has a slot in a sample array. In theory this is
// depreciated because I'm caching the slots inside the Texture Object
void Texture::bindToSlot(const uint32_t slot) const
{
  glBindTextureUnit(slot, m_rendererId);
}
// bind texture to cached slot, clear the slot value
void Texture::bindAndClearSlot()
{
  glBindTextureUnit(m_slot, m_rendererId);
  m_slot = 0;
}
void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, m_rendererId); }
// Delete the texture, note that this doesn't modify the texture slot array
// inside the Renderer. That task should be done elsewhere
Texture::~Texture()
{
  if (m_rendererId != 0) {
    PLOG_I("Deleted texture id = {}", m_rendererId);
    glDeleteTextures(1, &m_rendererId);
    Renderer2d::removeTexture(*this);
  }
}
Texture Texture::clone()
{
  return Texture(m_path, m_width, m_height, m_dataFormat, m_internalFormat,
                 m_rendererId);
}
Texture::Texture(const char *path, uint32_t width, uint32_t height,
                 uint32_t dataFormat, uint32_t internalFormat,
                 uint32_t rendererId)
    : m_path(path), m_width(width), m_height(height), m_dataFormat(dataFormat),
      m_internalFormat(internalFormat), m_rendererId(rendererId) {};

Texture::Texture(Texture &&other) noexcept
    : m_path(std::move(other.m_path)), m_width(other.m_width),
      m_height(other.m_height), m_dataFormat(other.m_dataFormat),
      m_internalFormat(other.m_internalFormat), m_rendererId(other.m_rendererId)
{
  other.m_rendererId = 0; // prevent double delete
}

Texture &Texture::operator=(Texture &&other) noexcept
{
  if (this != &other) {
    m_path = std::move(other.m_path);
    m_width = other.m_width;
    m_height = other.m_height;
    m_dataFormat = other.m_dataFormat;
    m_internalFormat = other.m_internalFormat;
    m_rendererId = other.m_rendererId;

    other.m_rendererId =
        0; // prevent deleting texture case the remaining texture is deleted
  }
  return *this;
}

} // namespace pain
