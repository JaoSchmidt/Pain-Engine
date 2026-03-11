/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RectBatch.cpp
#include "CoreRender/Renderer/BatchRect.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "platform/ContextBackend.h"
#include <iostream>

namespace pain
{

RectBatch RectBatch::create()
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
  return RectBatch{
      std::move(*VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(Vertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::Float2, "a_TexCoord"},
              {ShaderDataType::UByte4, "a_Color", true},
              {ShaderDataType::Float, "a_TexIndex"},
              {ShaderDataType::Float, "a_TilingFactor"},
          })),
      std::move(*IndexBuffer::createIndexBuffer(indices.data(), MaxIndices))};
}
RectBatch::RectBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),         //
      vao(*VertexArray::createVertexArray(vbo, ib)),    //
      ptrInit(std::make_unique<Vertex[]>(MaxVertices)), //
      ptr(ptrInit.get())                                //
{};

void RectBatch::resetPtr()
{
  indexCount = 0;
  ptr = ptrInit.get();
}
void RectBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void RectBatch::flush(Texture **textures, uint32_t textureCount)
{
  if (!indexCount)
    return;
  // sortByDrawOrder();
  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - ptrInit.get());
  vbo.setData(ptrInit.get(), count * sizeof(Vertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  ib.bind();
  backend::drawIndexed(vao, indexCount * IndiceSize);
#ifndef NDEBUG
  drawCount++;
#endif
}

void RectBatch::allocateRect(const glm::mat4 &transform, const Color &tintColor,
                             const float tilingFactor, const float textureIndex,
                             const std::array<glm::vec2, 4> &textureCoordinate)
{
  PROFILE_FUNCTION();
  constexpr glm::vec4 RectVertexPositions[4] = {
      glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, 0.5f, 0.f, 1.f),
      glm::vec4(-0.5f, 0.5f, 0.f, 1.f),
  };
  for (unsigned i = 0; i < 4; i++) {
    ptr->position = transform * RectVertexPositions[i];
    ptr->color = tintColor.value;
    ptr->texCoord = textureCoordinate[i];
    ptr->texIndex = textureIndex;
    ptr->tilingFactor = tilingFactor;
    ptr++;
  }
  // drawOrder[indexCount] = order;
  indexCount++;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
