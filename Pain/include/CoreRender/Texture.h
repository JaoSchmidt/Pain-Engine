#pragma once

#include "pch.h"

#include "Core.h"

namespace pain
{

class EXPORT Texture
{
public:
  Texture(const std::string &path);
  Texture(uint32_t width, uint32_t height);
  Texture(Texture &t);
  ~Texture();
  void bind(uint32_t slot = 0) const;
  void setData(void *data, uint32_t size);
  bool operator==(const Texture &other) const;

  std::string tempGet() const { return m_path; }
  const uint32_t getWidth() const { return m_width; }
  const uint32_t getHeight() const { return m_height; }

private:
  std::string m_path;
  uint32_t m_width, m_height;
  uint32_t m_dataFormat;
  uint32_t m_internalFormat;
  uint32_t m_rendererId;

  static SDL_Surface *flipVertical(SDL_Surface *surface)
  {
    SDL_Surface *result = SDL_CreateRGBSurface(
        surface->flags, surface->w, surface->h,
        surface->format->BytesPerPixel * 8, surface->format->Rmask,
        surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
    const auto pitch = surface->pitch;
    const auto pxlength = pitch * (surface->h - 1);
    auto pixels = static_cast<unsigned char *>(surface->pixels) + pxlength;
    auto rpixels = static_cast<unsigned char *>(result->pixels);
    for (auto line = 0; line < surface->h; ++line) {
      memcpy(rpixels, pixels, pitch);
      pixels -= pitch;
      rpixels += pitch;
    }
    return result;
  }
};

} // namespace pain
