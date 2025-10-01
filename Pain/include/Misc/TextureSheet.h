#pragma once
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Texture.h"
#include "pch.h"

namespace pain
{

class TextureSheet
{

public:
  static TextureSheet
  createTextureSheet(Texture &texture, uint nlinesX, uint ncolumnsY,
                     std::initializer_list<std::pair<int, int>> coords);
  static TextureSheet
  createTextureSheet(Texture &texture, float spriteWidth, float spriteHeight,
                     std::initializer_list<std::pair<int, int>> coords);
  void updateSurroundingChunks(glm::vec3 &playerPos);
  inline std::array<glm::vec2, 4> &operator[](ushort i)
  {
#ifndef NDEBUG
    if ((int)i > m_textureIds.size()) {
      PLOG_E("You are tyring to call an out of bounds texture id {} inside a "
             "textureSheeet",
             i);
    }
    i = i % m_textureIds.size();
#endif
    return m_textureIds[i];
  }

  const std::vector<std::vector<int>> &getDefaultMap() const;
  const std::vector<std::vector<int>> &getSceneryMap() const;
  const std::array<glm::vec2, 4> getTexCoord(int id) const
  {
    return m_textureIds.at(id);
  };
  const Texture &getTexture() const { return m_texture; }
  Texture &getTexture() { return m_texture; }
  const float getSpriteWidth() const { return m_spriteSize.x; }
  const float getSpriteHeight() const { return m_spriteSize.y; }
  const size_t size() const { return m_size; }
  MOVABLE(TextureSheet);
  NONCOPYABLE(TextureSheet)
  ~TextureSheet() = default;

private:
  std::vector<glm::vec2> m_chunksCoord;
  glm::vec2 m_spriteSize = {0.0f, 0.0f};
  size_t m_size = 1;
  std::reference_wrapper<Texture> m_texture;
  // this is what the renderer needs
  std::vector<std::array<glm::vec2, 4>> m_textureIds = {};

  void createVecFromMap();
  static std::array<glm::vec2, 4> createVecFromCoord(pain::Texture &texture,
                                                     float spriteWidth,
                                                     float spriteHeight, int x,
                                                     int y);
  static std::array<glm::vec2, 4> createVecFromCoord(pain::Texture &texture,
                                                     uint ndivisionX,
                                                     uint ndivisionY, int x,
                                                     int y);
  static std::array<glm::vec2, 4>
  createVecFromCoord(const char *atlasFilenameXML, int SubTextureId);
  glm::ivec2 getChunkCoordinate(glm::vec3 &playerCood);
  TextureSheet(Texture &texture, float spriteWidth, float spriteHeight,
               std::vector<std::array<glm::vec2, 4>> texturesIds, size_t size);
};
} // namespace pain
