
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
SphereBatch SphereBatch::create(uint32_t slices, uint32_t stacks,
                                Shader &shader)
{
  constexpr float radius = 0.5f;
  const uint32_t verticePerSphere = 2 + (slices - 2) * stacks;
  const uint32_t indicesPerSphere = slices * 3 +                // top cap
                                    (stacks - 3) * slices * 6 + // middle
                                    slices * 3;                 // bottom cap
  // -------- BUILD STATIC INDICES --------
  uint32_t *indices = new uint32_t[indicesPerSphere];

  const uint32_t northPole = 0;
  const uint32_t southPole = 1 + (stacks - 2) * slices;
  uint8_t i = 0; // indice only
  // Top cap
  for (uint32_t slice = 0; slice < slices; ++slice) {
    uint32_t next = (slice + 1) % slices;
    indices[i] = northPole;
    indices[i + 1] = 1 + next;
    indices[i + 2] = 1 + slice;
    i += 3;
  }

  // Middle Indices
  for (uint32_t stack = 0; stack < stacks - 3; ++stack) {
    uint32_t ringStart = 1 + stack * slices;
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
  uint32_t lastRing = southPole - slices;
  for (uint32_t slice = 0; slice < slices; ++slice) {
    uint32_t next = (slice + 1) % slices;
    indices[i] = lastRing + slice;
    indices[i + 1] = lastRing + next;
    indices[i + 2] = southPole;
    i += 3;
  }
  delete[] indices;

  // -------- BUILD STATIC VERTICES --------
  std::unique_ptr<Vertex[]> vertices =
      std::make_unique<Vertex[]>(verticePerSphere);
  Vertex *pVertex = vertices.get();

  pVertex->position = {0.f, radius, 0.f};
  pVertex->texCoord = {0.5f, 0.0f};
  ++pVertex;

  for (uint32_t stack = 1; stack < stacks - 1; ++stack) {
    const float v = fdiv(stack, stacks);
    const float flippedV = 1.0f - v;
    const float phi = glm::pi<float>() * v;

    const float y = radius * cos(phi);
    const float rTimesSin = radius * sin(phi);

    for (uint32_t slice = 0; slice < slices; ++slice) {
      const float u = fdiv(slice, slices - 1);
      const float theta = glm::two_pi<float>() * u;

      const float x = rTimesSin * cos(theta);
      const float z = rTimesSin * sin(theta);

      const glm::vec4 pos{x, y, z, 1.f};
      const glm::vec2 uv{1.f - u, flippedV};

      pVertex->position = pos;
      pVertex->texCoord = uv;
      ++pVertex;
    }
  }

  pVertex->position = {0.0f, -radius, 0.0f};
  pVertex->texCoord = {0.5f, 1.0f};
  // -------- SHADER --------
  // Shader shader =
  //     *Shader::createFromFile("resources/default/shaders/TexturePhong.glsl");

  int *samplers = new int[backend::getTMU()];
  for (int i = 0; i < backend::getTMUi(); ++i)
    samplers[i] = i;

  shader.bind();
  shader.uploadUniformIntArray("u_Textures", samplers, backend::getTMU());
  delete[] samplers;

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
      *IndexBuffer::createIndexBuffer(indices, indicesPerSphere),
      indicesPerSphere);
}

SphereBatch::SphereBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
                         IndexBuffer &&ib_,
                         uint32_t indicesPerSphere)
    : vbo(std::move(vbo_)),                                              //
      vboInstance(std::move(vboInstance_)),                              //
      ib(std::move(ib_)),                                                //
      vao(*VertexArray::createVertexArray(vbo, vboInstance, ib)),        //
      ptrInit(std::make_unique<SphereInstanceVertex[]>(MaxPolyhedrons)), //
      ptr(ptrInit.get()),                                                //
      m_indicesPerSphere(indicesPerSphere) {};

void SphereBatch::resetPtr()
{
  indexCount = 0;
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
  if (!indexCount)
    return;
  vao.bind();
  vbo.bind();

  vboInstance.bind();
  const uint32_t count = static_cast<uint32_t>(ptr - ptrInit.get());
  vboInstance.setData(ptrInit.get(), count * sizeof(Vertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  ib.bind();
  backend::drawIndexed(vao, indexCount * m_indicesPerSphere);
  // PLOG_T("Being flushed");
#ifndef NDEBUG
  drawCount++;
#endif
}

float inline fdiv(uint32_t divided, uint32_t divisor)
{
  return static_cast<float>(divided) / static_cast<float>(divisor);
}

void SphereBatch::allocateSphereUV(const glm::mat4 &transform,
                                   const Color &tintColor, float tilingFactor,
                                   float textureIndex)
{
  PROFILE_FUNCTION();

  // glm::mat3 normalMatrix =
  // glm::transpose(glm::inverse(glm::mat3(transform)));

  ptr->color = tintColor.value;
  ptr->texIndex = textureIndex;
  ptr->tilingFactor = tilingFactor;
  ptr->transform = transform;
  ++ptr;
  indexCount++;
#ifndef NDEBUG
  statsCount++;
#endif
}
} // namespace pain
