#include "Chunk.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/NativeScript.h"
#include "initialMap.h"
#include "pain.h"
#include <PerlinNoise.hpp>
#include <string>

ChunkEntity::ChunkEntity(pain::Scene *scene) : GameObject(scene)
{
  createComponents(pain::TransformComponent{},
                   pain::NativeScriptComponent("chunk nsc"));
}

bool ChunkController::isOutsideRadius(glm::ivec2 &playerChunkCoord, int radius)
{
  return m_offsetX < playerChunkCoord.x - radius ||
         m_offsetX >= playerChunkCoord.x + radius ||
         m_offsetY < playerChunkCoord.y - radius ||
         m_offsetY >= playerChunkCoord.y + radius;
}
void ChunkController::init(glm::ivec2 offSet, int chunkSize, MainMap *mainMap)
{
  pain::NativeScriptComponent &nsc =
      getComponent<pain::NativeScriptComponent>();
  m_offsetX = offSet.x;
  m_offsetY = offSet.y;
  // nsc.m_name = "chunk (" + std::to_string(offSet.x) + ", " +
  //              std::to_string(offSet.y) + ")";
  nsc.m_name = "chunk ()";
  m_mainMap = mainMap;
  m_chunkSize = chunkSize;
  m_data = generateTerrainMatrix(m_offsetX, m_offsetY);

  ASSERT(m_data.size() == chunkSize * chunkSize,
         "m_data of size {} does not have correct size {}", m_data.size(),
         chunkSize * chunkSize)
}
void ChunkController::onRender(double currentTime)
{
  for (int x = 0; x < m_chunkSize; x++) {
    for (int y = 0; y < m_chunkSize; y++) {
      int index = y * 32 + x;
      ASSERT(m_data[index] != 0, "Texture of index {} cannot be 0 in a chunk",
             index);
      pain::Renderer2d::drawQuad({1.f * (x + m_offsetX * m_chunkSize),
                                  1.f * (y + m_offsetY * m_chunkSize)},
                                 {1.f, 1.f}, {1.0f, 1.0f, 1.0f, 1.0f},
                                 m_mainMap->getTexture(), 1.0f,
                                 m_mainMap->getTexCoord(m_data[index]));
    }
  }
}

std::vector<int> ChunkController::generateTerrainMatrix(int offsetX,
                                                        int offsetY)
{
  const siv::PerlinNoise::seed_type seed = 123456u;
  const siv::PerlinNoise perlin{seed};
  const int areaSize = m_chunkSize * m_chunkSize;
  std::vector<int> matrix(areaSize);

  for (int x = 0; x < m_chunkSize; ++x) {
    for (int y = 0; y < m_chunkSize; ++y) {
      double noise = perlin.octave2D_01((x + m_offsetX * m_chunkSize) * 0.1,
                                        (y + m_offsetY * m_chunkSize) * 0.1, 4);
      int index = y * m_chunkSize + x; // Compute 1D index

      if (noise < 0.3) {
        matrix[index] = 36; // dirt 1
      } else if (noise < 0.45) {
        matrix[index] = 37; // dirt 2
      } else if (noise < 0.525) {
        matrix[index] = 10; // grass 1
      } else if (noise < 0.6) {
        matrix[index] = 11; // grass 2
      } else if (noise < 0.75) {
        matrix[index] = 35; // grass-rock
      } else if (noise < 0.85) {
        matrix[index] = 32; // rock 1
      } else if (noise < 0.9) {
        matrix[index] = 33; // rock 2
      } else {
        matrix[index] = 34; // rock 3
      }
    }
  }

  return matrix;
}
