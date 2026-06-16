
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSphere.cpp
#include "CoreRender/Renderer/BatchSphere.h"
#include "Debugging/Profiling.h"
#include "glm/gtc/constants.hpp"
#include "platform/ContextBackend.h"

namespace pain
{
float inline fdiv(uint32_t divided, uint32_t divisor)
{
  return static_cast<float>(divided) / static_cast<float>(divisor);
}
SphereBatch SphereBatch::create(uint32_t slices, uint32_t stacks,
                                std::string name)
{
  constexpr float radius = 0.5F;
  const uint32_t verticePerSphere = (stacks - 2) * slices +     // Slices
                                    2 * slices;                 // Poles
  const uint32_t indicesPerSphere = slices * 3 +                // top cap
                                    (stacks - 3) * slices * 6 + // middle
                                    slices * 3;                 // bottom cap
  // -------- BUILD STATIC INDICES --------
  std::unique_ptr<uint32_t[]> indices =
      std::make_unique<uint32_t[]>(indicesPerSphere);

  const uint32_t northPoleStart = 0;
  const uint32_t firstRingStart = slices;
  const uint32_t southPoleStart = slices + (stacks - 2) * slices;
  uint32_t i = 0; // indice only
  // Top cap
  for (uint32_t slice = 0; slice < slices; ++slice) {
    uint32_t next = (slice + 1) % slices;
    indices[i] = northPoleStart + slice;
    indices[i + 1] = firstRingStart + next;
    indices[i + 2] = firstRingStart + slice;
    i += 3;
  }

  // Middle Indices
  for (uint32_t stack = 0; stack < stacks - 3; ++stack) {
    uint32_t ringStart = firstRingStart + stack * slices;
    uint32_t nextRingStart = ringStart + slices;

    for (uint32_t slice = 0; slice < slices; ++slice) {
      uint32_t next = (slice + 1) % slices;

      uint32_t v0 = ringStart + slice;
      uint32_t v1 = nextRingStart + slice;
      uint32_t v2 = nextRingStart + next;
      uint32_t v3 = ringStart + next;

      indices[i] = v0;
      indices[i + 1] = v1;
      indices[i + 2] = v2;

      indices[i + 3] = v2;
      indices[i + 4] = v3;
      indices[i + 5] = v0;
      i += 6;
    }
  }

  // Bottom cap
  uint32_t lastRingStart = firstRingStart + (stacks - 3) * slices;
  for (uint32_t slice = 0; slice < slices; ++slice) {
    uint32_t next = (slice + 1) % slices;
    indices[i] = lastRingStart + slice;
    indices[i + 1] = lastRingStart + next;
    indices[i + 2] = southPoleStart + slice;
    i += 3;
  }

  // -------- BUILD STATIC VERTICES --------
  std::unique_ptr<Vertex[]> vertices =
      std::make_unique<Vertex[]>(verticePerSphere);
  Vertex *pVertex = vertices.get();

  // pVertex->position = {0.F, radius, 0.F};
  // pVertex->texCoord = {0.5F, 1.F};
  // ++pVertex;

  for (uint32_t slice = 0; slice < slices; ++slice) {
    float u = fdiv(slice, slices);

    pVertex->position = {0.F, radius, 0.F};
    pVertex->texCoord = {1.F - u, 1.F};
    ++pVertex;
  }

  for (uint32_t stack = 1; stack < stacks - 1; ++stack) {
    const float v = fdiv(stack, stacks);
    const float flippedV = 1.0F - v;
    const float phi = glm::pi<float>() * v;

    const float y = radius * cos(phi);
    const float rTimesSin = radius * sin(phi);

    for (uint32_t slice = 0; slice < slices; ++slice) {
      const float u = fdiv(slice, slices - 1);
      const float theta = glm::two_pi<float>() * u;

      const float x = rTimesSin * cos(theta);
      const float z = rTimesSin * sin(theta);

      const glm::vec4 pos{x, y, z, 1.F};
      const glm::vec2 uv{1.F - u, flippedV};

      pVertex->position = pos;
      pVertex->texCoord = uv;
      ++pVertex;
    }
  }

  for (uint32_t slice = 0; slice < slices; ++slice) {
    float u = fdiv(slice, slices);

    pVertex->position = {0.F, -radius, 0.F};
    pVertex->texCoord = {1.F - u, 0.F};
    ++pVertex;
  }
  // pVertex->position = {0.0F, -radius, 0.0F};
  // pVertex->texCoord = {0.5F, 0.0F};

  return SphereBatch(
      *VertexBuffer::createStaticVertexBuffer(
          vertices.get(), verticePerSphere * sizeof(Vertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::Float2, "a_TexCoord"},
          }),
      *VertexBuffer::createVertexBuffer(
          MaxPolyhedrons * sizeof(SphereInstanceVertex),
          {
              // Instances
              {ShaderDataType::UByte4, "a_Color", true, true},
              {ShaderDataType::Float, "a_TexIndex", false, true},
              {ShaderDataType::Float, "a_TilingFactor", false, true},
              {ShaderDataType::Mat4, "a_Transform", false, true},
          }),
      *IndexBuffer::createIndexBuffer(indices.get(), indicesPerSphere),
      indicesPerSphere, verticePerSphere, std::move(name));
}

SphereBatch::SphereBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
                         IndexBuffer &&ib_, uint32_t indicesPerSphere,
                         uint32_t verticesPerSphere,
                         std::string name)
    : vbo(std::move(vbo_)),                                              //
      vboInstance(std::move(vboInstance_)),                              //
      ib(std::move(ib_)),                                                //
      vao(*VertexArray::createVertexArray(vbo, vboInstance, ib)),        //
      ptrInit(std::make_unique<SphereInstanceVertex[]>(MaxPolyhedrons)), //
      ptr(ptrInit.get()),                                                //
      m_indicesPerSphere(indicesPerSphere),
      m_verticesPerSphere(verticesPerSphere), m_name(std::move(name)) {};

void SphereBatch::resetPtr()
{
  m_count = 0;
  ptr = ptrInit.get();
}
void SphereBatch::resetAll()
{
  resetPtr();
  // PLOG_T("Being reset");
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void SphereBatch::flush(Texture **textures, uint32_t textureCount)
{
  if (m_count == 0)
    return;
  vao.bind();
  vbo.bind();

  vboInstance.bind();
  const uint32_t count = static_cast<uint32_t>(ptr - ptrInit.get());
  vboInstance.setData(ptrInit.get(), count * sizeof(SphereInstanceVertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  ib.bind();
  backend::drawIndexedInstanced(vao, m_indicesPerSphere, m_count);
#ifndef NDEBUG
  drawCount++;
#endif
}

void SphereBatch::allocateSphereUV(const glm::mat4 &transform,
                                   const Color &tintColor, float tilingFactor,
                                   float textureIndex)
{
  PROFILE_FUNCTION();
  ptr->color = tintColor.value;
  ptr->texIndex = textureIndex;
  ptr->tilingFactor = tilingFactor;
  ptr->transform = transform;
  ++ptr;
  m_count++;
#ifndef NDEBUG
  statsCount++;
#endif
}
} // namespace pain
