/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// QuadBatch.cpp
#include "CoreRender/Renderer/BatchQuad.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "platform/ContextBackend.h"
#include <iostream>

namespace pain
{

QuadBatch QuadBatch::create()
{
  constexpr unsigned int indices[] = {0, 1, 2, 2, 3, 0};
  constexpr glm::vec3 VertexPositions[4] = {
      glm::vec3(-0.5F, -0.5F, 0.F),
      glm::vec3(0.5F, -0.5F, 0.F),
      glm::vec3(0.5F, 0.5F, 0.F),
      glm::vec3(-0.5F, 0.5F, 0.F),
  };
  constexpr glm::vec2 FaceUVs[4] = {
      {0.0F, 0.0F}, // bottom-left
      {1.0F, 0.0F}, // bottom-right
      {1.0F, 1.0F}, // top-right
      {0.0F, 1.0F}, // top-left
  };

  std::unique_ptr<BatchVertex[]> vertices =
      std::make_unique<BatchVertex[]>(VerticesPerQuad);
  BatchVertex *pVertex = vertices.get();
  for (uint32_t i = 0; i < VerticesPerQuad; i++) {
    pVertex->position = VertexPositions[i];
    pVertex->texCoord = FaceUVs[i];
    pVertex++;
  }

  return QuadBatch{
      std::move(*VertexBuffer::createStaticVertexBuffer(
          vertices.get(), sizeof(BatchVertex) * VerticesPerQuad,
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::Float2, "a_TexCoord"},
          })),
      std::move(*VertexBuffer::createVertexBuffer(
          MaxPolygons * sizeof(InstanceVertex),
          {
              {ShaderDataType::UByte4, "a_Color", true, true},
              {ShaderDataType::Float, "a_TexIndex", false, true},
              {ShaderDataType::Float, "a_TilingFactor", false, true},
              {ShaderDataType::Mat4, "a_Transform", false, true},
          })), //
      std::move(*IndexBuffer::createIndexBuffer(
          indices, sizeof(indices) / sizeof(indices[0]))) //
  };
}
QuadBatch::QuadBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
                     IndexBuffer &&ib_)
    : vbo(std::move(vbo_)), instanceVBO(std::move(vboInstance_)),
      ib(std::move(ib_)),                                         //
      vao(*VertexArray::createVertexArray(vbo, instanceVBO, ib)), //
      ptrInit(std::make_unique<InstanceVertex[]>(MaxPolygons)),   //
      ptr(ptrInit.get())                                          //
{};

void QuadBatch::resetPtr()
{
  m_count = 0;
  ptr = ptrInit.get();
}
void QuadBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void QuadBatch::flush(Texture **textures, uint32_t textureCount, Shader *shader)
{
  if (m_count == 0)
    return;
  vao.bind();
  vbo.bind();

  instanceVBO.bind();
  instanceVBO.setData(ptrInit.get(), m_count * sizeof(InstanceVertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  shader->bind();
  ib.bind();
  backend::drawIndexedInstanced(vao, 6, m_count);
#ifndef NDEBUG
  drawCount++;
#endif
}

void QuadBatch::allocateQuad(const glm::mat4 &transform, const Color &tintColor,
                             const float tilingFactor, const float textureIndex)
{
  PROFILE_FUNCTION();
  // *ptr = {
  //     tintColor.value, //
  //     textureIndex,    //
  //     tilingFactor,    //
  //     transform,       //
  // };
  ptr->color = tintColor.value;
  ptr->texIndex = textureIndex;
  ptr->tilingFactor = tilingFactor;
  ptr->transform = transform;
  ptr++;

  // drawOrder[indexCount] = order;
  m_count++;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
