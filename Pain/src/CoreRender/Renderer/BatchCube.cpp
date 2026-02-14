/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchCube.cpp
#include "platform/ContextBackend.h"
#include "CoreRender/Renderer/BatchCube.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include <iostream>

namespace pain
{

CubeBatch CubeBatch::create()
{
  std::vector<uint32_t> indices(MaxIndices);
  for (uint32_t i = 0, offset = 0; i < MaxIndices; i += 6, offset += 4) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
  }

  Shader shader =
      *Shader::createFromFile("resources/default/shaders/Texture.glsl");

  // Set texture samplers once
  int *samplers = new int[backend::getTMU()];
  for (int i = 0; i < backend::getTMUi(); i++)
    samplers[i] = i;

  shader.bind();
  shader.uploadUniformIntArray("u_Textures", samplers, backend::getTMU());
  delete[] samplers;

  // PLOG_T("Being created");
  return CubeBatch{
      std::move(*VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(Vertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::UByte4, "a_Color", true},
              {ShaderDataType::Float2, "a_TexCoord"},
              {ShaderDataType::Float, "a_TexIndex"},
              {ShaderDataType::Float, "a_TilingFactor"},
          })),
      std::move(*IndexBuffer::createIndexBuffer(indices.data(), MaxIndices)),
      std::move(shader)};
}
CubeBatch::CubeBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),         //
      vao(*VertexArray::createVertexArray(vbo, ib)),    //
      shader(std::move(shader_)),                       //
      ptrInit(std::make_unique<Vertex[]>(MaxVertices)), //
      ptr(ptrInit.get())                                //
// drawOrder(std::vector<int>(MaxPolygons)),           //
// sortBuffer(std::make_unique<Vertex[]>(MaxVertices)) //
{};

void CubeBatch::resetPtr()
{
  indexCount = 0;
  ptr = ptrInit.get();
}
void CubeBatch::resetAll()
{
  resetPtr();
  // PLOG_T("Being reset");
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void CubeBatch::flush(Texture **textures, uint32_t textureCount)
{
  if (!indexCount)
    return;
  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - ptrInit.get());
  vbo.setData(ptrInit.get(), count * sizeof(Vertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  shader.bind();
  ib.bind();
  backend::drawIndexed(vao, indexCount * IndicesPerCube);
  // PLOG_T("Being flushed");
#ifndef NDEBUG
  drawCount++;
#endif
}
void CubeBatch::allocateCube(const glm::mat4 &transform, const Color &tintColor,
                             const float tilingFactor, const float textureIndex,
                             const std::array<glm::vec2, 4> &textureCoordinate)
{
  PROFILE_FUNCTION();
  constexpr uint8_t CubeFaces[6][4] = {
      {0, 1, 2, 3}, // front
      {4, 5, 6, 7}, // back
      {0, 4, 7, 3}, // left
      {1, 5, 6, 2}, // right
      {3, 2, 6, 7}, // top
      {0, 1, 5, 4}, // bottom
  };
  constexpr glm::vec4 CubeVertexPositions[8] = {
      {-0.5f, -0.5f, -0.5f, 1.0f}, // 0
      {0.5f, -0.5f, -0.5f, 1.0f},  // 1
      {0.5f, 0.5f, -0.5f, 1.0f},   // 2
      {-0.5f, 0.5f, -0.5f, 1.0f},  // 3
      {-0.5f, -0.5f, 0.5f, 1.0f},  // 4
      {0.5f, -0.5f, 0.5f, 1.0f},   // 5
      {0.5f, 0.5f, 0.5f, 1.0f},    // 6
      {-0.5f, 0.5f, 0.5f, 1.0f},   // 7
  };

  // Correct Cube Allocation
  // first quad
  for (uint32_t face = 0; face < 6; face++) {
    const uint32_t color = tintColor.value;

    for (uint32_t i = 0; i < 4; i++) {
      uint8_t vertexIndex = CubeFaces[face][i];
      ptr->position = transform * CubeVertexPositions[vertexIndex];
      ptr->color = color;
      ptr->texCoord = textureCoordinate[i];
      ptr->texIndex = textureIndex;
      ptr->tilingFactor = tilingFactor;
      ptr++;
    }
  }
  indexCount++;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
