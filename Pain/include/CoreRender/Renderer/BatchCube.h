/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchCube.h
#pragma once

#include "ContextBackend.h"
#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
namespace pain
{

struct CubeVertex {
  glm::vec3 position;
  uint32_t color;
  glm::vec2 texCoord;
  float texIndex;
  float tilingFactor;
};

struct CubeBatch {
  using Vertex = CubeVertex;
  // quads
  static constexpr uint32_t IndicesPerQuad = 6;
  static constexpr uint32_t VerticesPerQuad = 4;

  // cube
  static constexpr uint32_t QuadsPerCube = 6;
  static constexpr uint32_t VerticesPerCube =
      VerticesPerQuad * QuadsPerCube; // 24
  static constexpr uint32_t IndicesPerCube =
      IndicesPerQuad * QuadsPerCube; // 36
  static constexpr uint32_t MaxPolyhedrons = 5000;
  static constexpr uint32_t MaxVertices = MaxPolyhedrons * VerticesPerCube;
  static constexpr uint32_t MaxIndices = MaxPolyhedrons * IndicesPerCube;
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

  static CubeBatch create();

  void allocateCube(const glm::mat4 &transform, const Color &tintColor,
                    const float tilingFactor, const float textureIndex,
                    const std::array<glm::vec2, 4> &textureCoordinate);
  void resetAll();
  void resetPtr();
  void flush(Texture **textures, uint32_t textureCount);

private:
  CubeBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_);
  // void swapQuadVertices(uint32_t sortedIndex, uint32_t unsortedIndex);
  // void sortByDrawOrder();
};
} // namespace pain
