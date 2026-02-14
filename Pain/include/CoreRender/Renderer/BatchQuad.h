/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// QuadBatch.h
#pragma once

#include "ContextBackend.h"
#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
namespace pain
{

struct QuadVertex {
  glm::vec3 position;
  uint32_t color;
  glm::vec2 texCoord;
  float texIndex;
  float tilingFactor;
};

struct QuadBatch {
  using Vertex = QuadVertex;
  static constexpr uint32_t IndiceSize = 6;
  static constexpr uint32_t VerticesPerQuad = 4;
  static constexpr uint32_t MaxPolygons = 5000;
  static constexpr uint32_t MaxVertices = MaxPolygons * 4;
  static constexpr uint32_t MaxIndices = MaxPolygons * IndiceSize;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  IndexBuffer ib;
  VertexArray vao;
  Shader shader;

  std::unique_ptr<Vertex[]> ptrInit;
  Vertex *ptr = nullptr;
  // std::vector<int> drawOrder;
  uint32_t indexCount = 0; // works for both draw order indexes and gpu indices

  // std::unique_ptr<Vertex[]> sortBuffer;

  static QuadBatch create();

  void allocateQuad(const glm::mat4 &transform, const Color &tintColor,
                    const float tilingFactor, const float textureIndex,
                    const std::array<glm::vec2, 4> &textureCoordinate);
  void resetAll();
  void resetPtr();
  void flush(Texture **textures, uint32_t textureCount);

private:
  QuadBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_);
  // void swapQuadVertices(uint32_t sortedIndex, uint32_t unsortedIndex);
  // void sortByDrawOrder();
};
} // namespace pain
