/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RectBatch.h
#pragma once

#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Buffers/VertexArray.h"
#include "CoreRender/Renderer/Colors.h"
namespace pain
{

struct RectVertex {
  glm::vec3 position;
  glm::vec2 texCoord;
  uint32_t color;
  float texIndex;
  float tilingFactor;
};

struct RectBatch {
  using Vertex = RectVertex;
  static constexpr uint32_t IndiceSize = 6;
  static constexpr uint32_t VerticesPerRect = 4;
  static constexpr uint32_t MaxPolygons = 5000;
  static constexpr uint32_t MaxVertices = MaxPolygons * 4;
  static constexpr uint32_t MaxIndices = MaxPolygons * IndiceSize;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  IndexBuffer ib;
  VertexArray vao;

  std::unique_ptr<Vertex[]> ptrInit;
  Vertex *ptr = nullptr;
  // std::vector<int> drawOrder;
  uint32_t indexCount = 0; // works for both draw order indexes and gpu indices

  // std::unique_ptr<Vertex[]> sortBuffer;

  static RectBatch create(Shader *shader);

  void allocateRect(const glm::mat4 &transform, const Color &tintColor,
                    const float tilingFactor, const float textureIndex,
                    const std::array<glm::vec2, 4> &textureCoordinate);
  void resetAll();
  void resetPtr();
  void flush(Texture **textures, uint32_t textureCount);

private:
  RectBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_);
  // void swapRectVertices(uint32_t sortedIndex, uint32_t unsortedIndex);
  // void sortByDrawOrder();
};
} // namespace pain
