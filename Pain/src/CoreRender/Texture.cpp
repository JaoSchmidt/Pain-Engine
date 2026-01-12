// Texture.cpp
#include "CoreRender/Texture.h"
#include "Core.h"

#include "CoreFiles/LogWrapper.h"
#include "SDL_image.h"
#include "TextureBackend.h"
#include <cstdint>
#include <optional>

namespace pain
{

/*
 * Sets a dump texture
 */
std::optional<Texture> Texture::createTexture(const char *name, uint32_t width,
                                              uint32_t height,
                                              ImageFormat format)
{
  backend::TextureCreateInfo info{name, width, height, format};

  uint32_t id = backend::createTexture(info);
  if (!id)
    return std::nullopt;

  return Texture(name, width, height, format, id);
}

std::optional<Texture> Texture::createTexture(const char *path, bool clamp)
{

  std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> surface(
      IMG_Load(path), SDL_FreeSurface);

  if (!surface) {
    PLOG_E("Failed to load texture {}", path);
    return {};
  }
  ImageFormat dataFormat;
  if (surface->format->BytesPerPixel == 3) {
    dataFormat = ImageFormat::RGB8;
  } else if (surface->format->BytesPerPixel == 4) {
    dataFormat = ImageFormat::RGBA8;
  }

  backend::TextureFromFileInfo info{path, surface.get(), clamp};
  uint32_t id = backend::createTextureFromFile(info);
  if (!id)
    return std::nullopt;

  return Texture(path, static_cast<unsigned>(surface.get()->w),
                 static_cast<unsigned>(surface.get()->h), dataFormat, id);
}

void Texture::bind() const { backend::bindTexture(m_textureId); }

// bind texture unit i.e. has a slot in a sample array. In theory this is
// depreciated because I'm caching the slots inside the Texture Object
void Texture::bindToSlot(uint32_t slot) const
{
  backend::bindTextureToSlot(m_textureId, slot);
}

// bind texture to cached slot, clear the slot value
void Texture::bindAndClearSlot()
{
  backend::bindTextureToSlot(m_textureId, m_slot);
  m_slot = 0;
}
void Texture::setData(const void *data, uint32_t size)
{
  backend::setTextureData(m_textureId, m_width, m_height, m_dataFormat, data,
                          size);
}

// Delete the texture, note that this doesn't modify the texture slot array
// inside the Renderer. That task should be done elsewhere
Texture::~Texture()
{
  if (m_textureId)
    backend::destroyTexture(m_textureId);
}

bool Texture::operator==(const Texture &other) const
{
  return m_textureId == other.m_textureId;
}
Texture Texture::clone()
{
  return Texture(m_path.c_str(), m_width, m_height, m_dataFormat, m_textureId);
}
Texture::Texture(const char *path, uint32_t width, uint32_t height,
                 ImageFormat dataFormat, uint32_t rendererId)
    : m_path(path), m_width(width), m_height(height), m_dataFormat(dataFormat),
      m_textureId(rendererId) {};

Texture::Texture(Texture &&other) noexcept
    : m_path(other.m_path), m_width(other.m_width), m_height(other.m_height),
      m_dataFormat(other.m_dataFormat), m_textureId(other.m_textureId)
{
  other.m_textureId = 0; // prevent double delete
}

Texture &Texture::operator=(Texture &&other) noexcept
{
  if (this != &other) {
    m_path = other.m_path;
    m_width = other.m_width;
    m_height = other.m_height;
    m_dataFormat = other.m_dataFormat;
    m_textureId = other.m_textureId;

    other.m_textureId =
        0; // prevent deleting texture case the remaining texture is deleted
  }
  return *this;
}

} // namespace pain
