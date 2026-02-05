/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSpray.h
#pragma once
#include "Assets/DeltaTime.h"
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"

namespace pain
{
struct ParticleVertex {
  glm::vec2 position; // -0.5 .. 0.5
  // glm::vec2 texCoord;
};
struct InstanceParticleVertex {
  glm::vec2 normal;
  float startTime = 0;
  glm::vec2 emitStart;
};

struct SprayBatch {
  using Vertex = InstanceParticleVertex;
  static constexpr uint32_t MaxPolygons = 1000;
  static constexpr uint32_t MaxVertices = MaxPolygons * 4;
  static constexpr uint32_t MaxIndices = MaxPolygons * 6;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  VertexBuffer instanceVBO;
  IndexBuffer ib;
  VertexArray vao;
  Shader shader;

  std::unique_ptr<Vertex[]> cpuBuffer;
  Vertex *ptr = nullptr;
  uint32_t instanceCount = 0;

  static SprayBatch create();

  void resetAll();
  void resetPtr();
  void flush();

  void allocateSprayParticles(const glm::vec2 &normal,
                              const DeltaTime startTime,
                              const glm::vec2 &emitStart);

private:
  SprayBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
             IndexBuffer &&ib_, Shader &&shader);
};

} // namespace pain
