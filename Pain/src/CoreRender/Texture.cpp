#include "CoreRender/Texture.h"

#include "CoreFiles/LogWrapper.h"
#include "CoreFiles/ResourceManagerSing.h"

namespace pain
{

Texture::Texture(const std::string &path)
{
  SDL_Surface *surface = Resources::getInstance()->getSurface(path);

  glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererId);
  glBindTexture(GL_TEXTURE_2D, m_rendererId);

  int mode;
  if (surface->format->BytesPerPixel == 3) {
    mode = GL_RGB;
  } else if (surface->format->BytesPerPixel == 4) {
    mode = GL_RGBA;
  } else {
    P_ASSERT(false, "Unsupported number of bytes per pixel {}",
             surface->format->BytesPerPixel);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode,
               GL_UNSIGNED_BYTE, surface->pixels);
}
void Texture::bind(uint32_t slot) const
{
  glBindTextureUnit(slot, m_rendererId);
}
Texture::~Texture() { glDeleteTextures(1, &m_rendererId); }
} // namespace pain
