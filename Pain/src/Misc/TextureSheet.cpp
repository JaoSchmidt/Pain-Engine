/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "Misc/TextureSheet.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "glm/fwd.hpp"
#include <utility>

namespace pain
{
// This assumes that
// 1. the spites are all squares inside a matrix and
// 2. you know the size of the sprites
std::array<glm::vec2, 4> createVecFromCoord(pain::Texture &texture,
                                            float spriteWidth,
                                            float spriteHeight, int x, int y,
                                            float texel)
{
  float m_spriteMargin = 1.0f;
  const float texW = static_cast<float>(texture.getWidth());
  const float texH = static_cast<float>(texture.getHeight());

  float spriteW = spriteWidth;
  float spriteH = spriteHeight;
  float fx = static_cast<float>(x);
  float fy = static_cast<float>(y);

  // Compute base UVs coordinates (or STs)
  float uMin = (fx * (spriteW + m_spriteMargin)) / texW;
  float vMin = (fy * (spriteH + m_spriteMargin)) / texH;
  float uMax = ((fx + 1) * spriteW + fx * m_spriteMargin) / texW;
  float vMax = ((fy + 1) * spriteH + fy * m_spriteMargin) / texH;

  // Small padding inward (half a texel)
  float texelX = texel / texW;
  float texelY = texel / texH;

  return {
      glm::vec2(uMin + texelX, vMin + texelY), // Bottom-left
      glm::vec2(uMax - texelX, vMin + texelY), // Bottom-right
      glm::vec2(uMax - texelX, vMax - texelY), // Top-right
      glm::vec2(uMin + texelX, vMax - texelY)  // Top-left
  };
}

std::array<glm::vec2, 4> createVecFromCoord(const char *atlasFilenameXML,
                                            int SubTextureId)
{
  PLOG_E("Arguments are not supported yet: atlasFilenameXML = {}, SubTextureId "
         "= {}",
         atlasFilenameXML, SubTextureId);
  return {glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0)};
}

// ---------------------------------------------------------- //
// Texture Sheet creation
// ---------------------------------------------------------- //

// This assumes that
// 1. the atlas is a matrix
// 2. you know the size of each sprite inside the matrix
TextureSheet TextureSheet::createWithDimensions(
    Texture &texture, float spriteWidth, float spriteHeight,
    std::initializer_list<std::pair<int, int>> coords, float padding)
{
  std::vector<std::array<glm::vec2, 4>> textureIds = {};
  textureIds.reserve(coords.size());
  for (auto [x, y] : coords) {
    textureIds.push_back(
        createVecFromCoord(texture, spriteWidth, spriteHeight, x, y, padding));
  }
  return TextureSheet(texture, spriteWidth, spriteHeight, textureIds,
                      coords.size());
}

// TODO:  NEEDS TESTING
// This assumes that
// 1. the atlas is a matrix
// 2. you know how much columns and lines inside the atlas
TextureSheet TextureSheet::createWithDivisions(
    Texture &texture, unsigned nlinesX, unsigned ncolumnsY,
    std::initializer_list<std::pair<int, int>> coords, float padding)
{
  std::vector<std::array<glm::vec2, 4>> textureIds = {};
  float spriteW = (float)texture.getWidth() / (float)ncolumnsY;
  float spriteH = (float)texture.getHeight() / (float)nlinesX;
  PLOG_I("TextureSheet - Each texture is (SpriteW, SpriteH) = ({},{})", spriteW,
         spriteH);
  for (auto [x, y] : coords) {
    auto texCoords =
        createVecFromCoord(texture, spriteW, spriteH, x, y, padding);
    textureIds.push_back(texCoords);
  }
  return TextureSheet(texture, spriteW, spriteH, textureIds, coords.size());
}

TextureSheet::TextureSheet(Texture &texture, float spriteWidth,
                           float spriteHeight,
                           std::vector<std::array<glm::vec2, 4>> textureIds,
                           size_t size)
    : m_spriteSize({spriteWidth, spriteHeight}), m_size(size),
      m_texture(texture), m_textureIds(std::move(textureIds)) {};

} // namespace pain
