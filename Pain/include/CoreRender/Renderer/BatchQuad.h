/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// QuadBatch.h
#pragma once

#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Buffers/VertexArray.h"
#include "CoreRender/Renderer/Misc.h"
namespace pain
{

struct QuadVertex {
  glm::vec3 position;
  glm::vec2 texCoord;
};

struct InstanceQuadVertex {
  uint32_t color;
  float texIndex;
  float tilingFactor;
  glm::mat4 transform;
};

struct QuadBatch {
  using BatchVertex = QuadVertex;
  using InstanceVertex = InstanceQuadVertex;
  static constexpr uint32_t IndiceSize = 6;
  static constexpr uint32_t VerticesPerQuad = 4;
  static constexpr uint32_t MaxPolygons = 5000;
  static constexpr uint32_t MaxVertices = MaxPolygons * 4;
  static constexpr uint32_t MaxIndices = MaxPolygons * IndiceSize;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  VertexBuffer instanceVBO;
  IndexBuffer ib;
  VertexArray vao;

  std::unique_ptr<InstanceVertex[]> ptrInit;
  InstanceVertex *ptr = nullptr;
  // std::vector<int> drawOrder;
  uint32_t m_count = 0; // works for both draw order indexes and gpu indices

  // std::unique_ptr<Vertex[]> sortBuffer;

  static QuadBatch create();

  void allocateQuad(const glm::mat4 &transform, const Color &tintColor,
                    const float tilingFactor, const float textureIndex);
  void resetAll();
  void resetPtr();
  void flush(Texture **textures, uint32_t textureCount);

private:
  QuadBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
            IndexBuffer &&ib_);
  // void swapQuadVertices(uint32_t sortedIndex, uint32_t unsortedIndex);
  // void sortByDrawOrder();
};
} // namespace pain
