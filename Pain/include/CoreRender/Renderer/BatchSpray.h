/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


// SprayBatch.h
#pragma once
#include "Assets/DeltaTime.h"
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

struct ParticleVertex {
  glm::vec2 position;
  glm::vec2 offset;
  glm::vec2 normal;
  float time;
  float rotationSpeed;
};

struct SprayBatch {
  using Vertex = ParticleVertex;
  static constexpr uint32_t MaxPolygons = 1000;
  static constexpr uint32_t MaxVertices = MaxPolygons * 4;
  static constexpr uint32_t MaxIndices = MaxPolygons * 6;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  IndexBuffer ib;
  VertexArray vao;
  Shader shader;

  std::unique_ptr<Vertex[]> cpuBuffer;
  Vertex *ptr = nullptr;
  uint32_t indexCount = 0;

  static SprayBatch create();
  void resetAll();
  void resetPtr();
  void flush();

  void allocateSprayParticles(const glm::vec2 &position,
                              const glm::vec2 &offset, const glm::vec2 &normal,
                              const DeltaTime startTime,
                              const float rotationSpeed);

private:
  SprayBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader);
};

} // namespace pain
