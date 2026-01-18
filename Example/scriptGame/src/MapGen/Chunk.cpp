/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "MapGen/Chunk.h"
#include "Assets/ManagerFile.h"
#include "Assets/ManagerTexture.h"
#include "MapGen/MainGen.h"
#include "Physics/MovementComponent.h"

#include <PerlinNoise.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define FILE_FORMAT "resources/textures/chunks/chunk_{}_{}.png"

void blitTileRGBA(const unsigned char *atlasPixels, int atlasWidth,
                  int atlasHeight, unsigned char *outPixels, int outWidth,
                  int outHeight, int dstX, int dstY, int tileSizeX,
                  int tileSizeY, const std::array<glm::vec2, 4> &uv)
{
  // Convert UVs to pixel coordinates in atlas
  int srcX = static_cast<int>(uv[0].x * atlasWidth);
  int srcY = static_cast<int>(uv[0].y * atlasHeight);

  for (int x = 0; x < tileSizeX; ++x) {
    for (int y = 0; y < tileSizeY; ++y) {
      // Flip Y when reading from atlas
      int atlasIndex = ((srcY + y) * atlasWidth + (srcX + x)) * 4;
      int outIndex = ((dstY + y) * outWidth + (dstX + x)) * 4;
      outPixels[outIndex + 0] = atlasPixels[atlasIndex + 0];
      outPixels[outIndex + 1] = atlasPixels[atlasIndex + 1];
      outPixels[outIndex + 2] = atlasPixels[atlasIndex + 2];
      outPixels[outIndex + 3] = atlasPixels[atlasIndex + 3];
    }
  }
}

bool saveChunkAsPNG(const std::vector<int> &tileIds, int numDiv,
                    pain::TextureSheet &spriteSheet, const char *filename)
{
  pain::Texture &atlas = spriteSheet.getTexture();

  if (!atlas.m_pixels) {
    PLOG_W("Trying to save texture as png, but texture isn't on the CPU! Try "
           "loading with keepOnCPUMemory = true");
    return false;
  }
  int tilePixelSizeX = spriteSheet.getSpriteWidth();
  int tilePixelSizeY = spriteSheet.getSpriteHeight();

  const int outWidth = numDiv * tilePixelSizeX;
  const int outHeight = numDiv * tilePixelSizeY;

  std::vector<unsigned char> outputPixels(outWidth * outHeight * 4, 255);

  const unsigned char *atlasPixels = atlas.m_pixels;
  const int atlasWidth = atlas.getWidth();
  const int atlasHeight = atlas.getHeight();

  for (int x = 0; x < numDiv; ++x) {
    for (int y = 0; y < numDiv; ++y) {
      int index = y * numDiv + x;
      int tileId = tileIds[index];

      auto uv = spriteSheet.getTexCoord(tileId);

      int dstX = x * tilePixelSizeX;
      // int dstY = (numDiv - y - 1) * tilePixelSizeY;
      int dstY = y * tilePixelSizeY;
      blitTileRGBA(atlasPixels, atlasWidth, atlasHeight, outputPixels.data(),
                   outWidth, outHeight, dstX, dstY, tilePixelSizeX,
                   tilePixelSizeY, uv);
    }
  }
  //
  // std::vector<unsigned char> flippedPixels(outWidth * outHeight * 4);
  // for (int y = 0; y < outHeight; ++y) {
  //   int srcRow = y * outWidth * 4;
  //   int dstRow = (outHeight - 1 - y) * outWidth * 4;
  //   std::memcpy(&flippedPixels[dstRow], &outputPixels[srcRow], outWidth * 4);
  // }
  stbi_flip_vertically_on_write(true);
  // Write PNG (stride = width * 4)
  int result = stbi_write_png(filename, outWidth, outHeight, 4,
                              outputPixels.data(), outWidth * 4);

  return result != 0;
}

std::vector<int> generateTerrainMatrix(int numDiv, int offSetX, int offSetY)
{
  const siv::PerlinNoise::seed_type seed = 123456u;
  const siv::PerlinNoise perlin{seed};
  const int areaSize = numDiv * numDiv;
  std::vector<int> matrix(areaSize);
  for (int x = 0; x < numDiv; ++x) {
    for (int y = 0; y < numDiv; ++y) {
      double noise = perlin.octave2D_01((x + offSetX * numDiv) * 0.1,
                                        (y + offSetY * numDiv) * 0.1, 4);
      int index = y * numDiv + x; // Compute 1D index

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

struct GenerateChunkJob {
  pain::Scene *scene;
  MainMap *mainMap;
  glm::ivec2 offSet;
  int numDiv;
  const char *file;

  void operator()()
  {
    std::vector<int> data = generateTerrainMatrix(numDiv, offSet.x, offSet.y);

    scene->enqueueMainThread([=, this, data = std::move(data)]() mutable {
      bool result =
          saveChunkAsPNG(data, numDiv, mainMap->getTextureSheet(), file);
      if (!result)
        PLOG_E("Failed to generate chunk texture");

      reg::Entity correctEntity = mainMap->getChunk(offSet.x, offSet.y);
      if (correctEntity == reg::Entity{-1})
        return;
      scene->getComponent<pain::SpriteComponent>(correctEntity)
          .setTexture(pain::TextureManager::getTexture(file));
    });
  }
};

reg::Entity Chunk::create(pain::Scene &scene, glm::ivec2 offSet, int numDiv,
                          float chunkSize, MainMap &mainMap)

{
  reg::Entity entity = scene.createEntity();

  std::string file = std::format(FILE_FORMAT, offSet.x, offSet.y);
  scene.createComponents(                                                //
      entity, pain::Transform2dComponent{glm::vec2(offSet) * chunkSize}, //
      pain::NativeScriptComponent{},                                     //
      pain::SpriteComponent{.m_size = glm::vec2(chunkSize),
                            .m_tex = &pain::TextureManager::createTexture(
                                file.c_str(), false, true, false, false)} //
  );

  pain::Scene::emplaceScript<Chunk::Script>(entity, scene, offSet, numDiv,
                                            mainMap, file.c_str());
  if (!pain::FileManager::existsFile(file))
    // Generate Chunk Texture
    scene.getThreadPool().enqueue([=, &mainMap, &scene]() {
      std::vector<int> data = generateTerrainMatrix(numDiv, offSet.x, offSet.y);

      scene.enqueueMainThread([=, &scene, &mainMap,
                               data = std::move(data)]() mutable {
        bool result = saveChunkAsPNG(data, numDiv, mainMap.getTextureSheet(),
                                     file.c_str());
        if (!result)
          PLOG_E("Failed to generate chunk texture");

        reg::Entity correctEntity = mainMap.getChunk(offSet.x, offSet.y);
        if (correctEntity == reg::Entity{-1})
          return;
        scene.getComponent<pain::SpriteComponent>(correctEntity)
            .setTexture(pain::TextureManager::createTexture(file.c_str()));
      });
    });
  // scene.getThreadPool().enqueue(
  //     GenerateChunkJob{&scene, &mainMap, offSet, numDiv, file.c_str()});

  return entity;
}

Chunk::Script::Script(reg::Entity entity, pain::Scene &scene, glm::ivec2 offSet,
                      int numDiv, MainMap &mainMap, const char *filename)
    : pain::WorldObject(entity, scene), m_offsetX(offSet.x),
      m_offsetY(offSet.y), m_filename(filename),
      m_spriteSheet(mainMap.getTextureSheet()) {};

bool Chunk::Script::isOutsideRadius(glm::ivec2 &playerChunkCoord, int radius)
{
  PROFILE_FUNCTION();
  return m_offsetX < playerChunkCoord.x - radius ||
         m_offsetX >= playerChunkCoord.x + radius ||
         m_offsetY < playerChunkCoord.y - radius ||
         m_offsetY >= playerChunkCoord.y + radius;
}

Chunk::Script::~Script()
{
  pain::TextureManager::deleteTexture(m_filename);
  // PLOG_T("Deleting chunk in ({},{})", m_offsetX, m_offsetY);
}
