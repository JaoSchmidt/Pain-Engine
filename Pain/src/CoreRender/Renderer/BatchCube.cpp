/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchCube.cpp
#include "CoreRender/Renderer/BatchCube.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "platform/ContextBackend.h"
#include <iostream>

// constexpr unsigned int indices[] = {
//     0,  1,  2,  2,  3,  0,  // front
//     4,  5,  6,  6,  7,  4,  // back
//     8,  9,  10, 10, 11, 8,  // left
//     12, 13, 14, 14, 15, 12, // right?
//     16, 17, 18, 18, 19, 16, // top?
//     20, 21, 22, 22, 23, 20, // bottom?
// };

namespace pain
{

CubeBatch CubeBatch::create(std::string name)
{
  std::vector<uint32_t> indices(IndicesPerCube);
  for (uint32_t i = 0, offset = 0; i < IndicesPerCube; i += 6, offset += 4) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
  }

  constexpr glm::vec2 FaceUVs[4] = {
      {0.0f, 0.0f}, // bottom-left
      {1.0f, 0.0f}, // bottom-right
      {1.0f, 1.0f}, // top-right
      {0.0f, 1.0f}, // top-left
  };
  constexpr uint32_t CubeFaces[6][4] = {
      {0, 1, 2, 3}, // front
      {4, 5, 6, 7}, // back
      {0, 4, 7, 3}, // left
      {1, 5, 6, 2}, // right
      {3, 2, 6, 7}, // top
      {0, 1, 5, 4}, // bottom
  };
  constexpr glm::vec3 CubeVertexPositions[8] = {
      {-0.5f, -0.5f, -0.5f}, // 0
      {0.5f, -0.5f, -0.5f},  // 1
      {0.5f, 0.5f, -0.5f},   // 2
      {-0.5f, 0.5f, -0.5f},  // 3
      {-0.5f, -0.5f, 0.5f},  // 4
      {0.5f, -0.5f, 0.5f},   // 5
      {0.5f, 0.5f, 0.5f},    // 6
      {-0.5f, 0.5f, 0.5f},   // 7
  };

  std::unique_ptr<Vertex[]> vertices =
      std::make_unique<Vertex[]>(VerticesPerCube);
  Vertex *pVertex = vertices.get();

  for (uint32_t face = 0; face < 6; face++) {
    for (uint32_t i = 0; i < 4; i++) {
      const uint32_t vertexIndex = CubeFaces[face][i];
      pVertex->position = CubeVertexPositions[vertexIndex];
      pVertex->texCoord = FaceUVs[i];
      pVertex++;
    }
  }

  // PLOG_T("Being created");
  return CubeBatch{
      *VertexBuffer::createStaticVertexBuffer(
          vertices.get(), VerticesPerCube * sizeof(Vertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::Float2, "a_TexCoord"},
          }),
      *VertexBuffer::createVertexBuffer(
          MaxPolyhedrons * sizeof(CubeInstanceVertex),
          {
              {ShaderDataType::UByte4, "a_Color", true, true},
              {ShaderDataType::Float, "a_TexIndex", false, true},
              {ShaderDataType::Float, "a_TilingFactor", false, true},
              {ShaderDataType::Mat4, "a_Transform", false, true},
          }),
      *IndexBuffer::createIndexBuffer(indices.data(), IndicesPerCube), name};
  // *IndexBuffer::createIndexBuffer(indices,
  //                                 sizeof(indices) / sizeof(indices[0]))};
}
CubeBatch::CubeBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
                     IndexBuffer &&ib_, std::string name)
    : vbo(std::move(vbo_)), vboInstance(std::move(vboInstance_)),      //
      ib(std::move(ib_)),                                              //
      vao(*VertexArray::createVertexArray(vbo, vboInstance, ib)),      //
      ptrInit(std::make_unique<CubeInstanceVertex[]>(MaxPolyhedrons)), //
      ptr(ptrInit.get()),                                              //
      m_name(name) {};

void CubeBatch::resetPtr()
{
  m_count = 0;
  ptr = ptrInit.get();
}
void CubeBatch::resetAll()
{

  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}
//
void CubeBatch::flush(Texture **textures, uint32_t textureCount)
{
  if (!m_count)
    return;
  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - ptrInit.get());
  vboInstance.bind();
  vboInstance.setData(ptrInit.get(), count * sizeof(CubeInstanceVertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  ib.bind();
  backend::drawIndexedInstanced(vao, IndicesPerCube, m_count);
#ifndef NDEBUG
  drawCount++;
#endif
}
void CubeBatch::allocateCube(const glm::mat4 &transform, const Color &tintColor,
                             const float tilingFactor, const float textureIndex)
{
  PROFILE_FUNCTION();
  ptr->color = tintColor.value;
  ptr->texIndex = textureIndex;
  ptr->tilingFactor = tilingFactor;
  ptr->transform = transform;
  ptr++;
  m_count++;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
