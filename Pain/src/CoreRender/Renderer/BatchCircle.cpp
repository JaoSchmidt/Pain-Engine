/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "ContextBackend.h"
#include "CoreRender/Renderer/BatchCircles.h"
#include "Debugging/Profiling.h"
#include <utility>
namespace pain
{

CircleBatch CircleBatch::create()
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
  return CircleBatch{
      *VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(CircleVertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::UByte4, "a_Color", true},
              {ShaderDataType::Float2, "a_Coord"},
          }),
      *IndexBuffer::createIndexBuffer(indices.data(), MaxIndices),
      *Shader::createFromFile("resources/default/shaders/Circles.glsl"),
  };
}
CircleBatch::CircleBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_,
                         Shader &&shader)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)), shader(std::move(shader)),
      cpuBuffer(std::make_unique<CircleVertex[]>(MaxVertices)),
      ptr(cpuBuffer.get()) {};

void CircleBatch::resetPtr()
{
  indexCount = 0;
  ptr = cpuBuffer.get();
}
void CircleBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void CircleBatch::flush()
{
  if (!indexCount)
    return;

  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - cpuBuffer.get());
  vbo.setData(cpuBuffer.get(), count * sizeof(CircleVertex));

  shader.bind();
  ib.bind();
  backend::drawIndexed(vao, indexCount);
#ifndef NDEBUG
  drawCount++;
#endif
}

void CircleBatch::allocateCircle(const glm::mat4 &transform,
                                 const Color &tintColor,
                                 const std::array<glm::vec2, 4> &coordinate)
{
  PROFILE_FUNCTION();
  constexpr glm::vec4 QuadVertexPositions[4] = {
      glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, 0.5f, 0.f, 1.f),
      glm::vec4(-0.5f, 0.5f, 0.f, 1.f),
  };
  for (unsigned i = 0; i < 4; i++) {
    ptr->position = transform * QuadVertexPositions[i];
    ptr->color = tintColor.value;
    ptr->coord = coordinate[i];
    ptr++;
  }
  indexCount += 6;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
