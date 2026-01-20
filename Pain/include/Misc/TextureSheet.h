/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "Core.h"
#include "CoreRender/Texture.h"
#include "pch.h"

namespace pain
{

/**
 * Represents a texture atlas subdivided into addressable sprite regions.
 *
 * A TextureSheet maps logical sprite indices to normalized texture
 * coordinates used by the renderer. Each entry stores four UV coordinates
 * defining a quad inside the underlying texture.
 *
 * Texture sheets can be created either by explicitly providing sprite
 * dimensions or by dividing the texture into a fixed grid.
 */
class TextureSheet
{
public:
  /**
   * Creates a texture sheet by dividing a texture into a fixed grid.
   *
   * The texture is split into nlinesX rows and ncolumnsY columns. Only the
   * sprite coordinates listed in coords are extracted and stored.
   *
   * @param texture Source texture atlas.
   * @param nlinesX Number of rows in the atlas.
   * @param ncolumnsY Number of columns in the atlas.
   * @param coords List of (x, y) sprite indices to extract.
   * @param padding Texel padding applied to avoid texture bleeding.
   */
  static TextureSheet
  createWithDivisions(Texture &texture, unsigned nlinesX, unsigned ncolumnsY,
                      std::initializer_list<std::pair<int, int>> coords,
                      float padding);

  /**
   * Creates a texture sheet using explicit sprite dimensions.
   *
   * Each sprite is assumed to have the given width and height in pixels.
   * Only the coordinates listed in coords are extracted.
   *
   * @param texture Source texture atlas.
   * @param spriteWidth Width of each sprite in pixels.
   * @param spriteHeight Height of each sprite in pixels.
   * @param coords List of (x, y) sprite indices to extract.
   * @param padding Texel padding applied to avoid texture bleeding.
   */
  static TextureSheet
  createWithDimensions(Texture &texture, float spriteWidth, float spriteHeight,
                       std::initializer_list<std::pair<int, int>> coords,
                       float padding);

  /** Updates internal chunk coordinates based on a world position. */
  void updateSurroundingChunks(glm::vec3 &playerPos);

  /**
   * Returns texture coordinates for a sprite index.
   *
   * If the index exceeds the available range, it wraps using modulo.
   */
  const inline std::array<glm::vec2, 4> &operator[](unsigned short i)
  {
    return m_textureIds[i % m_textureIds.size()];
  }

  /** Returns the default tile map associated with the sheet. */
  const std::vector<std::vector<int>> &getDefaultMap() const;

  /** Returns the scenery tile map associated with the sheet. */
  const std::vector<std::vector<int>> &getSceneryMap() const;

  /** Returns texture coordinates for a sprite without bounds wrapping. */
  inline const std::array<glm::vec2, 4> &getTexCoord(unsigned short id) const
  {
    return m_textureIds.operator[](id);
  };

  /** Returns the underlying texture atlas (const). */
  const Texture &getTexture() const { return m_texture; }

  /** Returns the underlying texture atlas. */
  Texture &getTexture() { return m_texture; }

  /** Returns sprite width in pixels. */
  int getSpriteWidth() const { return m_spriteSize.x; }

  /** Returns sprite height in pixels. */
  int getSpriteHeight() const { return m_spriteSize.y; }

  /** Returns number of stored sprite entries. */
  size_t size() const { return m_size; }

  MOVABLE(TextureSheet);
  NONCOPYABLE(TextureSheet)
  ~TextureSheet() = default;

private:
  std::vector<glm::vec2> m_chunksCoord = {};
  glm::ivec2 m_spriteSize = {0, 0};
  size_t m_size = 1;
  std::reference_wrapper<Texture> m_texture;

  /** Stores UV coordinates for each sprite quad. */
  std::vector<std::array<glm::vec2, 4>> m_textureIds = {};

  void createVecFromMap();
  glm::ivec2 getChunkCoordinate(glm::vec3 &playerCood);

  TextureSheet(Texture &texture, float spriteWidth, float spriteHeight,
               std::vector<std::array<glm::vec2, 4>> texturesIds, size_t size);
};

} // namespace pain
