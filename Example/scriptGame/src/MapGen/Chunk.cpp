#include "MapGen/Chunk.h"
#include "MapGen/MainGen.h"
#include "Physics/MovementComponent.h"

#include <PerlinNoise.hpp>

std::vector<int> generateTerrainMatrix(int chunkSize, int offSetX, int offSetY)
{
  const siv::PerlinNoise::seed_type seed = 123456u;
  const siv::PerlinNoise perlin{seed};
  const int areaSize = chunkSize * chunkSize;
  std::vector<int> matrix(areaSize);
  for (int x = 0; x < chunkSize; ++x) {
    for (int y = 0; y < chunkSize; ++y) {
      double noise = perlin.octave2D_01((x + offSetX * chunkSize) * 0.1,
                                        (y + offSetY * chunkSize) * 0.1, 4);
      int index = y * chunkSize + x; // Compute 1D index

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

reg::Entity Chunk::create(pain::Scene &scene, glm::ivec2 offset, int chunkSize,
                          MainMap &mainMap)

{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(                   //
      entity, pain::Transform2dComponent{}, //
      pain::NativeScriptComponent{}         //
  );
  scene.emplaceScript<Chunk::Script>(entity, scene, offset, chunkSize, mainMap);

  // PLOG_I("getting inside chunk.cpp {}", entity);
  // pain::NativeScriptComponent &nsc =
  scene.getComponent<pain::NativeScriptComponent>(entity);
  return entity;
}

Chunk::Script::Script(reg::Entity entity, pain::Scene &scene, glm::ivec2 offSet,
                      int chunkSize, MainMap &mainMap)
    : pain::WorldObject(entity, scene), m_spriteSheet(mainMap.getTextureSheet())

{
  m_offsetX = offSet.x;
  m_offsetY = offSet.y;
  // nsc.m_name = "chunk (" + std::to_string(offSet.x) + ", " +
  //              std::to_string(offSet.y) + ")";
  m_chunkSize = chunkSize;
  auto &pool = getThreadPool();

  pool.enqueue([=, &mainMap, &scene]() {
    std::vector<int> data =
        generateTerrainMatrix(chunkSize, offSet.x, offSet.y);

    scene.enqueueMainThread(
        [=, &scene, &mainMap, data = std::move(data)]() mutable {
          reg::Entity correctEntity = mainMap.getChunk(offSet.x, offSet.y);
          if (correctEntity == reg::Entity{-1})
            return;

          // NOTE: possible that correctEntity != entity
          scene.getNativeScript<Chunk::Script>(correctEntity)
              .setData(std::move(data));
        });
  });
}

bool Chunk::Script::isOutsideRadius(glm::ivec2 &playerChunkCoord, int radius)
{
  PROFILE_FUNCTION();
  return m_offsetX < playerChunkCoord.x - radius ||
         m_offsetX >= playerChunkCoord.x + radius ||
         m_offsetY < playerChunkCoord.y - radius ||
         m_offsetY >= playerChunkCoord.y + radius;
}
void Chunk::Script::onRender(pain::Renderer2d &renderer2d, bool isMinimized,
                             pain::DeltaTime currentTime)
{
  PROFILE_FUNCTION();
  if (m_data.size() == m_chunkSize * m_chunkSize)
    for (int x = 0; x < m_chunkSize; x++) {
      for (int y = 0; y < m_chunkSize; y++) {
        int index = y * 32 + x;
        ASSERT(m_data[index] != 0, "Texture of index {} cannot be 0 in a chunk",
               index);
        renderer2d.drawQuad({1.f * (x + m_offsetX * m_chunkSize),
                             1.f * (y + m_offsetY * m_chunkSize)},
                            {1.f, 1.f}, {1.0f, 1.0f, 1.0f, 1.0f},
                            m_spriteSheet.getTexture(), 1.0f,
                            m_spriteSheet.getTexCoord(m_data[index]));
      }
    }
}
