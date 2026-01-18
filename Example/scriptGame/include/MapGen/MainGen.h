/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

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
class MainMap
{
public:
  static MainMap create(float spriteWidth, float spriteHeight,
                        glm::vec2 &playerPos, int chunkNum, int radius,
                        float chunkSize);

  NONCOPYABLE(MainMap);
  NONMOVABLE(MainMap);
  void onCreate(pain::Scene &scene);
  void updateSurroundingChunks(const glm::vec2 &playerPos, pain::Scene &scene);

  const std::vector<std::vector<int>> &getDefaultMap() const;
  const std::vector<std::vector<int>> &getSceneryMap() const;
  pain::TextureSheet &getTextureSheet();

  bool chunkExists(int x, int y) const;
  reg::Entity getChunk(int x, int y) const;

private:
  int m_radius = 2;
  int m_numDiv = 32;
  float m_chunkSize = 4.f;
  glm::ivec2 m_chunkAt;
  std::map<std::pair<int, int>, reg::Entity> m_chunks;
  pain::TextureSheet &m_spriteSheet;

  void createVecFromMap();
  MainMap(int radius, int chunkNum, float chunkSize, glm::ivec2 chunkAt,
          float spriteWidth, float spriteHeight);
};
