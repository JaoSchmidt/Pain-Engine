/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// TriBatch.h
#pragma once

#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Buffers/VertexArray.h"
#include "CoreRender/Renderer/Colors.h"
namespace pain
{

struct TriVertex {
  glm::vec3 position;
  uint32_t color;
};

struct TriBatch {
  using Vertex = TriVertex;
  static constexpr uint32_t MaxPolygons = 1000;
  static constexpr uint32_t MaxVertices = MaxPolygons * 3;
  static constexpr uint32_t MaxIndices = MaxPolygons * 3;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  IndexBuffer ib;
  VertexArray vao;

  std::unique_ptr<Vertex[]> cpuBuffer;
  Vertex *ptr = nullptr;
  uint32_t indexCount = 0;

  static TriBatch create(const Shader *shader);
  void resetAll();
  void resetPtr();
  void flush();
  void allocateTri(const glm::mat4 &transform, const Color &tintColor);

private:
  TriBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_);
};
} // namespace pain
