#pragma once

#include "pch.h"

#include "Core.h"

namespace pain
{
enum class ImageFormat { None = 0, R8, RGB8, RGBA8, RGBA32F };
class Texture
{
public:
  Texture(const std::string &path);
  Texture(uint32_t width, uint32_t height,
          ImageFormat format = ImageFormat::RGBA8);
  Texture(Texture &t);
  ~Texture();
  void bind(uint32_t slot = 0) const;
  void setData(const void *data, uint32_t size);
  bool operator==(const Texture &other) const;

  const uint32_t getRendererId() const { return m_rendererId; }
  const uint32_t getWidth() const { return m_width; }
  const uint32_t getHeight() const { return m_height; }

  MOVABLES(Texture);
  NONCOPYABLE(Texture);

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

  constexpr unsigned getInternalFormat(ImageFormat format) const
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
  constexpr unsigned getGLDataFormat(ImageFormat format) const
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
};

} // namespace pain
