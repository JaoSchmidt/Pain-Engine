
#include "MapGen/MainGen.h"
#include "Debugging/Profiling.h"
#include "MapGen/Chunk.h"
#include "glm/fwd.hpp"
#include "pain.h"
#include <cstdlib>
#include <utility>

using Chunk::Script;

glm::ivec2 getChunkCoordinate(const glm::vec2 &playerCoord, float chunkSize)
{
  return {static_cast<int>(
              std::floor((playerCoord.x + chunkSize / 2.0f) / chunkSize)),
          static_cast<int>(
              std::floor((playerCoord.y + chunkSize / 2.0f) / chunkSize))};
}

MainMap MainMap::create(float spriteWidth, float spriteHeight,
                        glm::vec2 &playerPos, int chunkNum, int radius,
                        float chunkSize)
{

  glm::ivec2 chunkAt = getChunkCoordinate(playerPos, chunkNum);
  return MainMap{radius,  chunkNum,    chunkSize,
                 chunkAt, spriteWidth, spriteHeight};
}

MainMap::MainMap(int radius, int chunkNum, float chunkSize, glm::ivec2 chunkAt,
                 float spriteWidth, float spriteHeight)
    : m_radius(radius), m_chunkSize(chunkSize), m_chunkAt(chunkAt),
      m_spriteSheet(pain::TextureManager::createWithDimensions(
          "terrain",
          "resources/textures/kenney_roguelike/Spritesheet/"
          "roguelikeSheet_transparent.png",
          spriteWidth, spriteHeight,
          {
              // lake
              {0, 6},   // 0. transparent alpha
              {45, 19}, // 1. gold coins (for testing)
              {2, 30},  // 2. lake northwest corner
              {3, 30},  // 3. lake north corner
              {4, 30},  // 4. lake northeast corner
              {2, 29},  // 5. lake west corner
              {4, 29},  // 6. lake east corner
              {2, 28},  // 7. lake southwest corner
              {3, 28},  // 8. lake south corner
              {4, 28},  // 9. lake southeast corner

              // terrain
              {5, 30}, // 10. grass 1
              {5, 29}, // 11. grass 2

              // paths
              {9, 23},  // 12. path vertical
              {9, 22},  // 13. path horizontal
              {7, 23},  // 14. path 90 degrees (1)
              {7, 22},  // 15. path 90 degrees (2)
              {8, 23},  // 16. path 90 degrees (3)
              {8, 22},  // 17. path 90 degrees (4)
              {5, 23},  // 18. path T-junction (1)
              {5, 22},  // 19. path T-junction (2)
              {6, 23},  // 20. path T-junction (3)
              {6, 22},  // 21. path T-junction (4)
              {11, 23}, // 22. path cross (X)
                        // lake open corners
              {0, 29},  // 23. lake southeast open corner
              {1, 29},  // 24. lake southwest open corner
              {0, 28},  // 25. lake northwest open corner
              {1, 28},  // 26. lake northeast open corner
                        // tree
              {16, 20}, // 27. pine tree top
              {16, 19}, // 28. pine tree bottom

              // lake
              {0, 30}, // 29. lake 1
              {1, 30}, // 30 lake 2
              {55, 7}, // 31 lake rock
              {7, 30}, // 32 rock 1
              {7, 29}, // 33 rock 2
              {7, 30}, // 34 rock 3
              {9, 29}, // 35 grass-rock
              {6, 30}, // 36 dirt 1
              {6, 29}  // 37 dirt 2
          },
          true)) {};

void MainMap::onCreate(pain::Scene &scene)
{
  for (int x = m_chunkAt.x - m_radius; x <= m_chunkAt.x + m_radius; x++) {
    for (int y = m_chunkAt.y - m_radius; y <= m_chunkAt.y + m_radius; y++) {
      reg::Entity e =
          Chunk::create(scene, {x, y}, m_numDiv, m_chunkSize, *this);
      m_chunks.emplace(std::make_pair(x, y), e);
    }
  }
}

// given the index x and y, return the four corners
void MainMap::updateSurroundingChunks(const glm::vec2 &playerPos,
                                      pain::Scene &scene)
{
  PROFILE_FUNCTION()
  if (m_chunkAt != getChunkCoordinate(playerPos, m_chunkSize)) {
    m_chunkAt = getChunkCoordinate(playerPos, m_chunkSize);
    for (auto it = m_chunks.begin(); it != m_chunks.end();) {
      Chunk::Script &cs = scene.getNativeScript<Chunk::Script>(it->second);

      if (cs.isOutsideRadius(m_chunkAt, m_radius + 1)) {
        scene.removeEntity(it->second);
        it = m_chunks.erase(it); // it now will be the next iterator
      } else {
        ++it;
      }
    }
    for (int x = m_chunkAt.x - m_radius; x <= m_chunkAt.x + m_radius; x++) {
      for (int y = m_chunkAt.y - m_radius; y <= m_chunkAt.y + m_radius; y++) {
        if (!m_chunks.contains({x, y})) {
          reg::Entity e =
              Chunk::create(scene, {x, y}, m_numDiv, m_chunkSize, *this);
          m_chunks.emplace(std::make_pair(x, y), e);
        }
      }
    }
  }
}

pain::TextureSheet &MainMap::getTextureSheet() { return m_spriteSheet; }

reg::Entity MainMap::getChunk(int x, int y) const
{
  const auto it = m_chunks.find({x, y});
  if (it != m_chunks.end())
    return it->second;
  return reg::Entity{-1};
}
bool MainMap::chunkExists(int x, int y) const
{
  return m_chunks.contains({x, y});
}
