/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Texture.cpp
#include "CoreRender/Texture.h"
#include "Core.h"

#include "CoreFiles/LogWrapper.h"

#include "TextureBackend.h"
#include <cstdint>
#include <iostream>
#include <optional>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

std::optional<Texture> Texture::createTexture(const char *path, bool clamp,
                                              bool keepOnCPUMemory,
                                              bool isError)
{
  backend::TextureFromFileInfo info{.path = path, .clampToEdge = clamp};
  stbi_set_flip_vertically_on_load(true);

  info.pixels = stbi_load(path, &info.width, &info.height, &info.channels, 0);

  if (!info.pixels) {
    if (isError)
      PLOG_E("Failed to load texture {} : {}", path, stbi_failure_reason());
    return std::nullopt;
  }

  ImageFormat dataFormat;
  if (info.channels == 3) {
    dataFormat = ImageFormat::RGB8;
  } else if (info.channels == 4) {
    dataFormat = ImageFormat::RGBA8;
  } else {
    PLOG_E("Unsupported channel count {} for {}", info.channels, path);
    return std::nullopt;
  }

  uint32_t id = backend::createTextureFromFile(info);
  if (!id)
    return std::nullopt;

  if (!keepOnCPUMemory) {
    stbi_image_free(info.pixels);
    info.pixels = nullptr;
  }
  return Texture(path, static_cast<unsigned>(info.width),
                 static_cast<unsigned>(info.height), dataFormat, id,
                 info.pixels);
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
  if (m_pixels)
    stbi_image_free(m_pixels);
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

Texture::Texture(const char *path, uint32_t width, uint32_t height,
                 ImageFormat dataFormat, uint32_t rendererId,
                 unsigned char *pixels)
    : m_pixels(pixels), m_path(path), m_width(width), m_height(height),
      m_dataFormat(dataFormat), m_textureId(rendererId) {};

Texture::Texture(Texture &&other) noexcept
    : m_pixels(other.m_pixels), m_path(other.m_path), m_width(other.m_width),
      m_height(other.m_height), m_dataFormat(other.m_dataFormat),
      m_textureId(other.m_textureId)
{
  other.m_textureId = 0;    // prevent double delete
  other.m_pixels = nullptr; // prevent double delete
}

Texture &Texture::operator=(Texture &&other) noexcept
{
  if (this != &other) {
    m_path = other.m_path;
    m_width = other.m_width;
    m_height = other.m_height;
    m_dataFormat = other.m_dataFormat;
    m_textureId = other.m_textureId;
    m_pixels = other.m_pixels;

    other.m_textureId =
        0; // prevent deleting texture case the remaining texture is deleted
    other.m_pixels = nullptr; // prevent deleting texture case the remaining
                              // texture is deleted
  }
  return *this;
}

} // namespace pain
