#include "initialMap.h"
#include "Chunk.h"
#include "Debugging/Profiling.h"
#include "glm/fwd.hpp"
#include "pain.h"
#include <cstdlib>
#include <utility>

MainMap::MainMap(float spriteWidth, float spriteHeight, glm::vec3 &playerPos,
                 pain::Scene *scene)
    : m_spriteSize({spriteWidth, spriteHeight}),
      m_spriteSheet(
          new pain::Texture("resources/textures/kenney_roguelike/Spritesheet/"
                            "roguelikeSheet_transparent.png"))
{
  // clang-format off
  // lake
  m_texturesIds.push_back(createVecFromCoord(0, 6)); // 0. transparent alpha
  m_texturesIds.push_back(createVecFromCoord(45, 19)); // 1. gold coins (for testing)
  m_texturesIds.push_back(createVecFromCoord(2, 30)); // 2. lake northwest corner
  m_texturesIds.push_back(createVecFromCoord(3, 30)); // 3. lake north corner
  m_texturesIds.push_back(createVecFromCoord(4, 30)); // 4. lake northeast corner
  m_texturesIds.push_back(createVecFromCoord(2, 29)); // 5. lake west corner
  m_texturesIds.push_back(createVecFromCoord(4, 29)); // 6. lake east corner
  m_texturesIds.push_back(createVecFromCoord(2, 28)); // 7. lake southwest corner
  m_texturesIds.push_back(createVecFromCoord(3, 28)); // 8. lake south corner
  m_texturesIds.push_back(createVecFromCoord(4, 28)); // 9. lake southeast corner

  // terrain
  m_texturesIds.push_back(createVecFromCoord(5, 30)); // 10. grass 1
  m_texturesIds.push_back(createVecFromCoord(5, 29)); // 11. grass 2

  // paths
  m_texturesIds.push_back(createVecFromCoord(9, 23));  // 12. path vertical
  m_texturesIds.push_back(createVecFromCoord(9, 22));  // 13. path horizontal
  m_texturesIds.push_back(createVecFromCoord(7, 23));  // 14. path 90 degrees (1)
  m_texturesIds.push_back(createVecFromCoord(7, 22));  // 15. path 90 degrees (2)
  m_texturesIds.push_back(createVecFromCoord(8, 23));  // 16. path 90 degrees (3)
  m_texturesIds.push_back(createVecFromCoord(8, 22));  // 17. path 90 degrees (4)
  m_texturesIds.push_back(createVecFromCoord(5, 23));  // 18. path T-junction (1)
  m_texturesIds.push_back(createVecFromCoord(5, 22));  // 19. path T-junction (2)
  m_texturesIds.push_back(createVecFromCoord(6, 23));  // 20. path T-junction (3)
  m_texturesIds.push_back(createVecFromCoord(6, 22));  // 21. path T-junction (4)
  m_texturesIds.push_back(createVecFromCoord(11, 23)); // 22. path cross (X)
  // lake open corners
  m_texturesIds.push_back(createVecFromCoord(0, 29)); // 23. lake southeast open corner
  m_texturesIds.push_back(createVecFromCoord(1, 29)); // 24. lake southwest open corner
  m_texturesIds.push_back(createVecFromCoord(0, 28)); // 25. lake northwest open corner
  m_texturesIds.push_back(createVecFromCoord(1, 28)); // 26. lake northeast open corner
  // tree
  m_texturesIds.push_back(createVecFromCoord(16, 20)); // 27. pine tree top
  m_texturesIds.push_back(createVecFromCoord(16, 19)); // 28. pine tree bottom

  // lake
  m_texturesIds.push_back(createVecFromCoord(0, 30)); // 29. lake 1
  m_texturesIds.push_back(createVecFromCoord(1, 30)); // 30 lake 2
  m_texturesIds.push_back(createVecFromCoord(55, 7)); // 31 lake rock
  m_texturesIds.push_back(createVecFromCoord(7, 30)); // 32 rock 1
  m_texturesIds.push_back(createVecFromCoord(7, 29)); // 33 rock 2
  m_texturesIds.push_back(createVecFromCoord(7, 30)); // 34 rock 3
  m_texturesIds.push_back(createVecFromCoord(9, 29)); // 35 grass-rock
  m_texturesIds.push_back(createVecFromCoord(6, 30)); // 36 dirt 1
  m_texturesIds.push_back(createVecFromCoord(6, 29)); // 37 dirt 2
  // clang-format on

  m_chunkAt = getChunkCoordinate(playerPos);
  m_chunks.reserve(2 * 2 * m_radius * m_radius);
  for (int x = m_chunkAt.x - m_radius; x < m_chunkAt.x + m_radius; ++x) {
    for (int y = m_chunkAt.y - m_radius; y < m_chunkAt.y + m_radius; ++y) {
      ChunkEntity &chunk =
          m_chunks.emplace(std::make_pair(x, y), scene).first->second;
      pain::NativeScriptComponent &nsc =
          chunk.getComponent<pain::NativeScriptComponent>();
      nsc.bind<ChunkController>();
      pain::initializeScript(scene, nsc, chunk.getEntity());
      ChunkController *cc = (ChunkController *)nsc.instance;
      cc->init({x, y}, m_chunkSize, this);
    }
  }
}
// given the index x and y, return the four corners
std::array<glm::vec2, 4> MainMap::createVecFromCoord(int x, int y)
{
  float m_spriteMargin = 1.0f;
  float texW = m_spriteSheet->getWidth();
  float texH = m_spriteSheet->getHeight();

  float spriteW = m_spriteSize.x;
  float spriteH = m_spriteSize.y;

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

glm::ivec2 MainMap::getChunkCoordinate(glm::vec3 &playerCood)
{
  PROFILE_FUNCTION();
  return {playerCood.x / m_chunkSize, playerCood.y / m_chunkSize};
}

void MainMap::updateSurroundingChunks(glm::vec3 &playerPos, pain::Scene *scene)
{
  PROFILE_FUNCTION()
  if (m_chunkAt != getChunkCoordinate(playerPos)) {
    glm::ivec2 dif = getChunkCoordinate(playerPos) - m_chunkAt;
    m_chunkAt = getChunkCoordinate(playerPos);
    for (auto it = m_chunks.begin(); it != m_chunks.end();) {
      ChunkController *cc = ((ChunkController *)(it->second)
                                 .getComponent<pain::NativeScriptComponent>()
                                 .instance);
      if (cc->isOutsideRadius(m_chunkAt, m_radius + 1)) {
        it = m_chunks.erase(it);
      } else {
        ++it;
      }
    }
    // reserve a column
    m_chunks.reserve(m_chunks.size() + abs(dif.x) * m_radius +
                     abs(dif.y) * m_radius);
    for (int x = m_chunkAt.x - m_radius; x < m_chunkAt.x + m_radius; ++x) {
      for (int y = m_chunkAt.y - m_radius; y < m_chunkAt.y + m_radius; ++y) {
        auto [pair, isInserted] = m_chunks.emplace(std::make_pair(x, y), scene);
        if (isInserted) {
          ChunkEntity &chunk = pair->second;
          pain::NativeScriptComponent &nsc =
              chunk.getComponent<pain::NativeScriptComponent>();
          nsc.bind<ChunkController>();
          pain::initializeScript(scene, nsc, chunk.getEntity());
          ChunkController *cc = (ChunkController *)nsc.instance;
          cc->init({x, y}, m_chunkSize, this);
        }
      }
    }
  }
}
