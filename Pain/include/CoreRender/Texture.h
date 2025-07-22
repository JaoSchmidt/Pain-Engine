#pragma once

#include "Core.h"

#include <SDL2/SDL_surface.h>
#include <cstdint>
#include <glad/gl.h>
#include <string>

namespace pain
{
enum class ImageFormat { None = 0, R8, RGB8, RGBA8, RGBA32F };
class Texture
{
public:
  Texture(const char *path, bool gl_clamp_to_edge = false);
  Texture(uint32_t width, uint32_t height,
          ImageFormat format = ImageFormat::RGBA8);
  ~Texture();
  void bindToSlot(uint32_t slot = 0) const;
  void bindAndClearSlot();
  void bind() const;
  void setData(const void *data, uint32_t size);
  bool operator==(const Texture &other) const;
  Texture clone();

  uint32_t getRendererId() const { return m_rendererId; }
  uint32_t getWidth() const { return m_width; }
  uint32_t getHeight() const { return m_height; }

  MOVABLE(Texture);
  NONCOPYABLE(Texture);
  // COPIES(Texture);

  // this variable should only be modified inside the renderer
  uint32_t m_slot = 0;

private:
  std::string m_path;
  uint32_t m_width, m_height;
  uint32_t m_dataFormat;
  uint32_t m_internalFormat;
  uint32_t m_rendererId;
  SDL_Surface m_surface;

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
  Texture(std::string &path, uint32_t width, uint32_t height,
          uint32_t dataFormat, uint32_t internalFormat, uint32_t rendererId);
};

} // namespace pain
