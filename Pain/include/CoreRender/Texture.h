/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** Texture.h */
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

/** GPU texture resource with optional CPU-side pixel ownership. */
class Texture
{
public:
  // ============================================================= //
  // **Creation**
  // ============================================================= //

  /** Loads a texture from disk. */
  static std::optional<Texture> createTexture(const char *path,
                                              bool clamp = false,
                                              bool keepOnCPUMemory = false,
                                              bool isError = true);

  /** Creates an empty texture with the given dimensions and format. */
  static std::optional<Texture>
  createTexture(const char *name, uint32_t width, uint32_t height,
                ImageFormat format = ImageFormat::RGBA8);

  ~Texture();

  // ============================================================= //
  // **Binding**
  // ============================================================= //

  /** Binds the texture to the currently active texture unit. */
  void bind() const;

  /** Binds the texture to a specific texture slot. */
  void bindToSlot(uint32_t slot = 0) const;

  /** Binds the texture to its cached slot and clears the slot value. */
  void bindAndClearSlot();

  // ============================================================= //
  // **Data**
  // ============================================================= //

  /** Uploads raw pixel data to the texture. */
  void setData(const void *data, uint32_t size);

  /** Returns true if both textures reference the same GPU resource. */
  bool operator==(const Texture &other) const;

  /** Creates a lightweight copy referencing the same GPU texture. */
  Texture clone();

  /** Returns the backend texture handle. */
  uint32_t getRendererId() const { return m_textureId; }

  /** Returns the texture width in pixels. */
  uint32_t getWidth() const { return m_width; }

  /** Returns the texture height in pixels. */
  uint32_t getHeight() const { return m_height; }

  Texture(Texture &&other) noexcept;
  Texture &operator=(Texture &&other) noexcept;
  Texture(const Texture &o) = delete;
  Texture &operator=(const Texture &o) = delete;

  // ============================================================= //
  // **Renderer-managed state**
  // ============================================================= //

  /** Cached texture slot managed by the renderer. */
  uint32_t m_slot = 0;

  /** Optional CPU-side pixel buffer when retained. */
  unsigned char *m_pixels = nullptr;

  /** File path or debug name. */
  std::string m_path = "NULL texture string";

private:
  uint32_t m_width = 1, m_height = 1;
  ImageFormat m_dataFormat = ImageFormat::R8;
  uint32_t m_textureId = 0;

  Texture(const char *path, uint32_t width, uint32_t height,
          ImageFormat dataFormat, uint32_t rendererId, unsigned char *pixels);
  Texture(const char *path, uint32_t width, uint32_t height,
          ImageFormat dataFormat, uint32_t rendererId);
};

} // namespace pain
