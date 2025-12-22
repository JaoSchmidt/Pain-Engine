// Texture.h
#pragma once

#include "Core.h"
#include "ImageFormat.h"

#include <SDL2/SDL_surface.h>
#include <cstdint>
#include <glad/gl.h>
#include <optional>
#include <string>

namespace pain
{
class Texture
{
public:
  static std::optional<Texture> createTexture(const char *path,
                                              bool clamp = false);
  static std::optional<Texture>
  createTexture(const char *name, uint32_t width, uint32_t height,
                ImageFormat format = ImageFormat::RGBA8);
  ~Texture();
  void bindToSlot(uint32_t slot = 0) const;
  void bindAndClearSlot();
  void bind() const;
  void setData(const void *data, uint32_t size);
  bool operator==(const Texture &other) const;
  Texture clone();

  uint32_t getRendererId() const { return m_textureId; }
  uint32_t getWidth() const { return m_width; }
  uint32_t getHeight() const { return m_height; }

  Texture(Texture &&other) noexcept;
  Texture &operator=(Texture &&other) noexcept;
  Texture(const Texture &o) = delete;
  Texture &operator=(const Texture &o) = delete;

  // this variable should only be modified inside the renderer
  uint32_t m_slot = 0;

  std::string m_path = "NULL texture string"; // or name
private:
  uint32_t m_width, m_height;
  ImageFormat m_dataFormat;
  uint32_t m_textureId;

  Texture(const char *path, uint32_t width, uint32_t height,
          ImageFormat dataFormat, uint32_t rendererId);
};

} // namespace pain
