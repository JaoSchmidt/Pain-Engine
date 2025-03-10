#include "Chunk.h"
#include <cstddef>
#include <pain.h>
#include <vector>

struct SheetCoord {
  int x;
  int y;
};

class MainMap
{
public:
  MainMap(float spriteWidth, float spriteHeight, glm::vec3 &playerPos,
          pain::Scene *scene);

  void updateSurroundingChunks(glm::vec3 &playerPos, pain::Scene *scene);

  const std::vector<std::vector<int>> &getDefaultMap() const;
  const std::vector<std::vector<int>> &getSceneryMap() const;
  const std::array<glm::vec2, 4> getTexCoord(int id) const
  {
    return m_texturesIds.at(id);
  };
  const float getSpriteWidth() const { return m_spriteSize.x; }
  const float getSpriteHeight() const { return m_spriteSize.y; }
  pain::Texture &getTexture() const { return *m_spriteSheet.get(); }

private:
  int m_radius = 2;
  int m_chunkSize = 32;
  glm::ivec2 m_chunkAt;
  std::unordered_map<std::pair<int, int>, ChunkEntity, ChunkHash> m_chunks;
  std::vector<glm::vec2> m_chunksCoord;
  std::array<glm::vec2, 4> createVecFromCoord(int x, int y);
  glm::vec2 m_spriteSize = {0.0f, 0.0f};
  std::unique_ptr<pain::Texture> m_spriteSheet;
  std::vector<std::array<glm::vec2, 4>> m_texturesIds = {};
  std::vector<std::vector<glm::vec2[4]>> m_texturesCoord = {};

  void createVecFromMap();
  glm::ivec2 getChunkCoordinate(glm::vec3 &playerCood);
};
