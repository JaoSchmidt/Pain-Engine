/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/BatchTri.h"
#include "Debugging/Profiling.h"
namespace pain
{

TriBatch TriBatch::create()
{
  // Indices (simple triangles)
  std::vector<uint32_t> indices(MaxIndices);
  for (uint32_t i = 0, offset = 0; i < MaxIndices; i += 3, offset += 3) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;
  }

  return TriBatch{
      *VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(Vertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::Float4, "a_Color"},
          }),
      *IndexBuffer::createIndexBuffer(indices.data(), MaxIndices),
      *Shader::createFromFile("resources/default/shaders/Triangles.glsl"),
  };
}

TriBatch::TriBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)), shader(std::move(shader_)),
      cpuBuffer(std::make_unique<Vertex[]>(MaxVertices)),
      ptr(cpuBuffer.get()) {};

void TriBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}
void TriBatch::resetPtr()
{
  indexCount = 0;
  ptr = cpuBuffer.get();
}
void TriBatch::flush()
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

void TriBatch::allocateTri(const glm::mat4 &transform,
                           const glm::vec4 &tintColor)
{
  constexpr glm::vec4 TriVertexPositions[3] = {
      glm::vec4(0.0f, 0.5f, 0.f, 1.f),
      glm::vec4(0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
  };
  PROFILE_FUNCTION();
  for (unsigned i = 0; i < 3; i++) {
    ptr->position = transform * TriVertexPositions[i];
    ptr->color = tintColor;
    ptr++;
  }
  indexCount += 3;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
