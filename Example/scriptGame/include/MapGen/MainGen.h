#pragma once
#include "Chunk.h"
#include <pain.h>
#include <vector>

struct SheetCoord {
  int x;
  int y;
};

// struct MapMapParams {
//   pain::Scene &scene;
//   float spriteWidth;
//   float spriteHeight;
//   glm::vec2 &playerPos;
//   int chunkSize;
//   int radius;
// };
class MainMap : pain::WorldObject
{
public:
  static MainMap create(pain::Scene &scene, float spriteWidth,
                        float spriteHeight, glm::vec2 &playerPos, int chunkSize,
                        int radius);

  NONCOPYABLE(MainMap);
  NONMOVABLE(MainMap);
  void onCreate();
  void updateSurroundingChunks(glm::vec2 &playerPos);

  const std::vector<std::vector<int>> &getDefaultMap() const;
  const std::vector<std::vector<int>> &getSceneryMap() const;
  pain::TextureSheet &getTextureSheet();

  bool chunkExists(int x, int y) const;
  reg::Entity getChunk(int x, int y) const;

private:
  int m_radius = 1;
  int m_chunkSize = 32;
  glm::ivec2 m_chunkAt;
  std::map<std::pair<int, int>, reg::Entity> m_chunks;
  pain::TextureSheet &m_spriteSheet;

  void createVecFromMap();
  MainMap(reg::Entity entity, pain::Scene &scene, int radius, int chunkSize,
          glm::ivec2 chunkAt, float spriteWidth, float spriteHeight);
};
