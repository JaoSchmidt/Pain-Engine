/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSphere.h
#pragma once

#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Buffers/VertexArray.h"
#include "CoreRender/Renderer/Misc.h"
namespace pain
{

struct SphereVertex {
  glm::vec3 position; // vertex position in the world
  glm::vec2 texCoord; // Texture coordinate in the UV map
};
struct SphereInstanceVertex {
  uint32_t color;     // Basic color mulitplied with the result texture
  float texIndex;     // index of the texture in the TMUs
  float tilingFactor; // directive for texture repetition
  glm::mat4 transform;
};

struct SphereBatch {
  using Vertex = SphereVertex;
  // Sphere
  static constexpr uint32_t MaxPolyhedrons = 200;

  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  VertexBuffer vboInstance;
  IndexBuffer ib;
  VertexArray vao;

  std::unique_ptr<SphereInstanceVertex[]> ptrInit;
  SphereInstanceVertex *ptr = nullptr;
  // std::vector<int> drawOrder;
  uint32_t indexCount = 0; // works for both draw order indexes and gpu indices
  // std::unique_ptr<Vertex[]> sortBuffer;

  static SphereBatch create(uint32_t slices, uint32_t stacks, Shader &shader);

  void allocateSphereUV(const glm::mat4 &transform, const Color &tintColor,
                        float tilingFactor, float textureIndex);
  void resetAll();
  void resetPtr();
  void flush(Texture **textures, uint32_t textureCount);

private:
  SphereBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
              IndexBuffer &&ib_, uint32_t indicesPerSphere);

  const uint32_t m_indicesPerSphere = 0;
  // void swapQuadVertices(uint32_t sortedIndex, uint32_t unsortedIndex);
  // void sortByDrawOrder();
};
} // namespace pain
