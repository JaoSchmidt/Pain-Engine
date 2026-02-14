/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSphere.h
#pragma once

#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
namespace pain
{

struct SphereVertex {
  glm::vec3 position;
  uint32_t color;
  glm::vec2 texCoord;
  float texIndex;
  float tilingFactor;
  glm::vec3 normal;
};

struct SphereBatch {
  using Vertex = SphereVertex;
  // Sphere
  static constexpr uint32_t MaxPolyhedrons = 200;

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

  static SphereBatch create(uint32_t slices, uint32_t stacks);

  void allocateSphereUV(const glm::mat4 &transform, const Color &tintColor,
                        float tilingFactor, float textureIndex);
  void resetAll();
  void resetPtr();
  void flush(Texture **textures, uint32_t textureCount);

  uint32_t m_maxIndices = 0;

private:
  SphereBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_,
              uint32_t maxVertices, uint32_t maxIndices, uint32_t slices,
              uint32_t stack, uint32_t indicesPerSphere,
              uint32_t verticesPerShpere);

  uint32_t m_indicesPerSphere = 0;
  uint32_t m_verticesPerSphere = 0;
  uint32_t m_slices = 0;
  uint32_t m_stacks = 0;
  // void swapQuadVertices(uint32_t sortedIndex, uint32_t unsortedIndex);
  // void sortByDrawOrder();
};
} // namespace pain
