#include "Misc/TextureSheet.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "glm/fwd.hpp"
#include <utility>

namespace pain
{

// This assumes that
// 1. the atlas is a matrix
// 2. you know the size of each sprite inside the matrix
TextureSheet TextureSheet::createTextureSheet(
    Texture &texture, float spriteWidth, float spriteHeight,
    std::initializer_list<std::pair<int, int>> coords)
{
  std::vector<std::array<glm::vec2, 4>> textureIds = {};
  for (auto [x, y] : coords) {
    textureIds.push_back(
        createVecFromCoord(texture, spriteWidth, spriteHeight, x, y));
  }
  return TextureSheet(texture, spriteWidth, spriteHeight, textureIds,
                      coords.size());
}

// TODO:  NEEDS TESTING
// This assumes that
// 1. the atlas is a matrix
// 2. you know how much columns and lines inside the atlas
TextureSheet TextureSheet::createTextureSheet(
    Texture &texture, uint nlinesX, uint ncolumnsY,
    std::initializer_list<std::pair<int, int>> coords)
{
  std::vector<std::array<glm::vec2, 4>> textureIds = {};
  float texW = texture.getWidth();
  float texH = texture.getHeight();

  float spriteW = texW / ncolumnsY;
  float spriteH = texH / nlinesX;
  PLOG_I("TextureSheet - Each texture is (SpriteW, SpriteH) = ({},{})", spriteW,
         spriteH);
  int idx = 0;
  for (auto [x, y] : coords) {
    auto texCoords = createVecFromCoord(texture, spriteW, spriteH, x, y);
    textureIds.push_back(texCoords);

    PLOG_I("TexId[{}] ({}, {}) -> "
           "({:.3f}, {:.3f}), ({:.3f}, {:.3f}), "
           "({:.3f}, {:.3f}), ({:.3f}, {:.3f})",
           idx, x, y, texCoords[0].x, texCoords[0].y, texCoords[1].x,
           texCoords[1].y, texCoords[2].x, texCoords[2].y, texCoords[3].x,
           texCoords[3].y);
    ++idx;
  }
  return TextureSheet(texture, spriteW, spriteH, textureIds, coords.size());
}

TextureSheet::TextureSheet(Texture &texture, float spriteWidth,
                           float spriteHeight,
                           std::vector<std::array<glm::vec2, 4>> textureIds,
                           size_t size)
    : m_spriteSize({spriteWidth, spriteHeight}), m_size(size),
      m_texture(texture), m_textureIds(std::move(textureIds)) {};
// This assumes that
// 1. the spites are all squares inside a matrix and
// 2. you know the size of the sprites
std::array<glm::vec2, 4>
TextureSheet::createVecFromCoord(pain::Texture &texture, float spriteWidth,
                                 float spriteHeight, int x, int y)
{
  float m_spriteMargin = 1.0f;
  float texW = texture.getWidth();
  float texH = texture.getHeight();

  float spriteW = spriteWidth;
  float spriteH = spriteHeight;

  // Compute base UVs coordinates (or STs)
  float uMin = (x * (spriteW + m_spriteMargin)) / texW;
  float vMin = (y * (spriteH + m_spriteMargin)) / texH;
  float uMax = ((x + 1) * spriteW + x * m_spriteMargin) / texW;
  float vMax = ((y + 1) * spriteH + y * m_spriteMargin) / texH;

  // Small padding inward (half a texel)
  float texelX = 0.5f / texW;
  float texelY = 0.5f / texH;

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
  PLOG_E("A undefined version of the function createVecFromCoord was called");
  return {glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0)};
}
} // namespace pain
