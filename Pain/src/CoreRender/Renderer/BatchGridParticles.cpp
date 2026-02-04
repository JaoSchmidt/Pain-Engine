/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/BatchGridParticles.h"
#include "ContextBackend.h"
#include "Debugging/Profiling.h"
namespace pain
{

GridParticleBatch GridParticleBatch::create()
{
  // Indices (quad-style)
  std::vector<uint32_t> indices(MaxIndices);
  for (uint32_t i = 0, offset = 0; i < MaxIndices; i += 6, offset += 4) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
  }

  return GridParticleBatch{
      *VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(GridParticleVertex),
          {
              {ShaderDataType::Float2, "a_Direction"},
              {ShaderDataType::UByte4, "a_Color", true},
          }),
      *IndexBuffer::createIndexBuffer(indices.data(), MaxIndices),
      *Shader::createFromFile("resources/default/shaders/GridParticles.glsl"),
  };
}
GridParticleBatch::GridParticleBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_,
                                     Shader &&shader)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)), shader(std::move(shader)),
      cpuBuffer(std::make_unique<GridParticleVertex[]>(MaxVertices)),
      ptr(cpuBuffer.get()) {};

void GridParticleBatch::resetPtr()
{
  indexCount = 0;
  ptr = cpuBuffer.get();
}
void GridParticleBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void GridParticleBatch::flush()
{
  if (!indexCount)
    return;

  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - cpuBuffer.get());
  vbo.setData(cpuBuffer.get(), count * sizeof(Vertex));

  shader.bind();
  ib.bind();
  backend::drawIndexed(vao, indexCount);
#ifndef NDEBUG
  drawCount++;
#endif
}

void GridParticleBatch::allocateGridParticles(Color color,
                                              const glm::vec2 &direction)
{
  PROFILE_FUNCTION();
  constexpr glm::vec2 SprayVertexPositions[4] = {
      glm::vec2(-0.5f, -0.5f),
      glm::vec2(0.5f, -0.5f),
      glm::vec2(0.5f, 0.5f),
      glm::vec2(-0.5f, 0.5f),
  };
  for (unsigned i = 0; i < 4; i++) {
    ptr->direction = direction;
    ptr->color = color.value;
    ptr++;
  }
  indexCount += 6;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
