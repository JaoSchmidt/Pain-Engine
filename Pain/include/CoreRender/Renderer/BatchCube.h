/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchCube.h
#pragma once

#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Buffers/VertexArray.h"
#include "CoreRender/Renderer/Colors.h"
namespace pain
{

// ------------ ULTIMATE GOAL

struct CubeVertex {
  glm::vec3 position;
  glm::vec2 texCoord;
};

struct CubeInstanceVertex {
  uint32_t color;
  float texIndex;
  float tilingFactor;
  glm::mat4 transform;
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
  VertexBuffer vboInstance;
  IndexBuffer ib;
  VertexArray vao;

  std::unique_ptr<CubeInstanceVertex[]> ptrInit;
  CubeInstanceVertex *ptr = nullptr;
  uint32_t m_count = 0; // works for both draw order indexes and gpu indices
  static CubeBatch create(std::string name, const Shader *shader);

  void allocateCube(const glm::mat4 &transform, const Color &tintColor,
                    float tilingFactor, float textureIndex);
  void resetAll();
  void resetPtr();
  void flush(Texture **textures, uint32_t textureCount);

private:
  std::string m_name;
  CubeBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_, IndexBuffer &&ib_,
            std::string name);
};

} // namespace pain
