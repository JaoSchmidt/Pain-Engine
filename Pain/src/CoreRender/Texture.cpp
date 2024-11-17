#include "CoreRender/Texture.h"

#include "CoreFiles/LogWrapper.h"
#include "CoreFiles/ResourceManagerSing.h"
#include "glad/gl.h"

namespace pain
{

bool Texture::operator==(const Texture &other) const
{
  return m_rendererId == other.m_rendererId;
}
/*
 * Sets a dump texture
 */
Texture::Texture(uint32_t width, uint32_t height, ImageFormat format)
    : m_width(width), m_height(height)
{
  m_internalFormat = getInternalFormat(format);
  m_dataFormat = getGLDataFormat(format);

  glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererId);
  glTextureStorage2D(m_rendererId, 1, m_internalFormat, m_width, m_height);

  glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

/*
 * Sets a texture given a specific texture image path
 */
Texture::Texture(const std::string &path)
{
  SDL_Surface *surface = Resources::getInstance()->getSurface(path);
  P_ASSERT(surface != nullptr, "Texture path \"{}\" was not found!", path);
  surface = flipVertical(surface);
  m_path = path;

  glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererId);
  glBindTexture(GL_TEXTURE_2D, m_rendererId);
  m_width = surface->w;
  m_height = surface->h;

  if (surface->format->BytesPerPixel == 3) {
    m_dataFormat = GL_RGB;
    m_internalFormat = GL_RGB8;
  } else if (surface->format->BytesPerPixel == 4) {
    m_dataFormat = GL_RGBA;
    m_internalFormat = GL_RGBA8;
  } else {
    P_ASSERT(false, "Unsupported number of bytes per pixel {}",
             surface->format->BytesPerPixel);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, m_dataFormat, surface->w, surface->h, 0,
               m_dataFormat, GL_UNSIGNED_BYTE, surface->pixels);
}
void Texture::setData(const void *data, uint32_t size)
{
  uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
  P_ASSERT_W(size == m_width * m_height * bpp, "Data must be entire texture!");
  glTextureSubImage2D(m_rendererId, 0, 0, 0, m_width, m_height, m_dataFormat,
                      GL_UNSIGNED_BYTE, data);
}
void Texture::bind(uint32_t slot) const
{
  glBindTextureUnit(slot, m_rendererId);
}
Texture::~Texture() { glDeleteTextures(1, &m_rendererId); }

} // namespace pain
