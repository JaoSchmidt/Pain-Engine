/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


// TriBatch.h
#pragma once

#include "ContextBackend.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
namespace pain
{

struct TriVertex {
  glm::vec3 position;
  glm::vec4 color;
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
  Shader shader;

  std::unique_ptr<Vertex[]> cpuBuffer;
  Vertex *ptr = nullptr;
  uint32_t indexCount = 0;

  static TriBatch create();
  void resetAll();
  void resetPtr();
  void flush();
  void allocateTri(const glm::mat4 &transform, const glm::vec4 &tintColor);

private:
  TriBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_);
};
} // namespace pain
