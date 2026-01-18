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

class TextureSheet
{

public:
  static TextureSheet
  createWithDivisions(Texture &texture, unsigned nlinesX, unsigned ncolumnsY,
                      std::initializer_list<std::pair<int, int>> coords,
                      float padding);
  static TextureSheet
  createWithDimensions(Texture &texture, float spriteWidth, float spriteHeight,
                       std::initializer_list<std::pair<int, int>> coords,
                       float padding);
  void updateSurroundingChunks(glm::vec3 &playerPos);

  // choose a specific texture coordenate by its index. If index > size, then
  // index = index % size
  const inline std::array<glm::vec2, 4> &operator[](unsigned short i)
  {
    return m_textureIds[i % m_textureIds.size()];
  }

  const std::vector<std::vector<int>> &getDefaultMap() const;
  const std::vector<std::vector<int>> &getSceneryMap() const;
  inline const std::array<glm::vec2, 4> &getTexCoord(unsigned short id) const
  {
    return m_textureIds.operator[](id);
  };
  const Texture &getTexture() const { return m_texture; }
  Texture &getTexture() { return m_texture; }
  int getSpriteWidth() const { return m_spriteSize.x; }
  int getSpriteHeight() const { return m_spriteSize.y; }
  size_t size() const { return m_size; }
  MOVABLE(TextureSheet);
  NONCOPYABLE(TextureSheet)
  ~TextureSheet() = default;

private:
  std::vector<glm::vec2> m_chunksCoord = {};
  glm::ivec2 m_spriteSize = {0, 0};
  size_t m_size = 1;
  std::reference_wrapper<Texture> m_texture;
  // this is what the renderer needs
  std::vector<std::array<glm::vec2, 4>> m_textureIds = {};

  void createVecFromMap();
  glm::ivec2 getChunkCoordinate(glm::vec3 &playerCood);
  TextureSheet(Texture &texture, float spriteWidth, float spriteHeight,
               std::vector<std::array<glm::vec2, 4>> texturesIds, size_t size);
};
} // namespace pain
