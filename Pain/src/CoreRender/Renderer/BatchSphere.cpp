
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSphere.cpp
#include "CoreRender/Renderer/BatchSphere.h"
#include "platform/ContextBackend.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "glm/gtc/constants.hpp"

namespace pain
{
SphereBatch SphereBatch::create(uint32_t slices, uint32_t stacks)
{
  const uint32_t verticePerSphere = 2 + (slices - 2) * stacks;
  const uint32_t indicesPerSphere = slices * 3 +                // top cap
                                    (stacks - 3) * slices * 6 + // middle
                                    slices * 3;                 // bottom cap
  const uint32_t maxVertices = verticePerSphere * MaxPolyhedrons;
  const uint32_t maxIndices = indicesPerSphere * MaxPolyhedrons;

  // -------- BUILD STATIC INDICES --------
  std::vector<uint32_t> indices;
  indices.reserve(maxIndices);

  const uint32_t northPole = 0;
  const uint32_t southPole = 1 + (stacks - 2) * slices;
  // Top cap
  for (uint32_t sphere = 0; sphere < MaxPolyhedrons; ++sphere) {
    uint32_t baseVertex = sphere * verticePerSphere;
    for (uint32_t slice = 0; slice < slices; ++slice) {
      uint32_t next = (slice + 1) % slices;
      indices.push_back(baseVertex + northPole);
      indices.push_back(baseVertex + 1 + next);
      indices.push_back(baseVertex + 1 + slice);
    }

    // Middle
    for (uint32_t stack = 0; stack < stacks - 3; ++stack) {
      uint32_t ringStart = 1 + stack * slices;
      uint32_t nextRingStart = ringStart + slices;

      for (uint32_t slice = 0; slice < slices; ++slice) {
        uint32_t next = (slice + 1) % slices;

        uint32_t v0 = ringStart + slice;
        uint32_t v1 = nextRingStart + slice;
        uint32_t v2 = nextRingStart + next;
        uint32_t v3 = ringStart + next;

        indices.push_back(baseVertex + v0);
        indices.push_back(baseVertex + v1);
        indices.push_back(baseVertex + v2);

        indices.push_back(baseVertex + v2);
        indices.push_back(baseVertex + v3);
        indices.push_back(baseVertex + v0);
      }
    }

    // Bottom cap
    uint32_t lastRing = southPole - slices;
    for (uint32_t slice = 0; slice < slices; ++slice) {
      uint32_t next = (slice + 1) % slices;
      indices.push_back(baseVertex + lastRing + slice);
      indices.push_back(baseVertex + lastRing + next);
      indices.push_back(baseVertex + southPole);
    }
  }
  // -------- SHADER --------
  Shader shader =
      *Shader::createFromFile("resources/default/shaders/TextureLight.glsl");

  int *samplers = new int[backend::getTMU()];
  for (int i = 0; i < backend::getTMUi(); ++i)
    samplers[i] = i;

  shader.bind();
  shader.uploadUniformIntArray("u_Textures", samplers, backend::getTMU());
  delete[] samplers;

  // -------- BUFFERS --------
  auto vbo = VertexBuffer::createVertexBuffer(
      maxVertices * sizeof(Vertex),
      {
          {ShaderDataType::Float3, "a_Position"},
          {ShaderDataType::UByte4, "a_Color", true},
          {ShaderDataType::Float2, "a_TexCoord"},
          {ShaderDataType::Float, "a_TexIndex"},
          {ShaderDataType::Float, "a_TilingFactor"},
          {ShaderDataType::Float3, "a_Normal"},
      });

  auto ibo = IndexBuffer::createIndexBuffer(
      indices.data(), static_cast<uint32_t>(indices.size()));

  return SphereBatch{std::move(*vbo), std::move(*ibo),  std::move(shader),
                     maxVertices,     maxIndices,       slices,
                     stacks,          indicesPerSphere, verticePerSphere};
}

SphereBatch::SphereBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_,
                         Shader &&shader_, uint32_t maxVertices,
                         uint32_t maxIndices, uint32_t slices, uint32_t stack,
                         uint32_t indicesPerSphere, uint32_t verticesPerShpere)
    : vbo(std::move(vbo_)),                          //
      ib(std::move(ib_)),                            //
      vao(*VertexArray::createVertexArray(vbo, ib)), //
      shader(std::move(shader_)),
      ptrInit(std::make_unique<Vertex[]>(maxVertices)), //
      ptr(ptrInit.get()),                               //
      m_maxIndices(maxIndices),                         //
      m_indicesPerSphere(indicesPerSphere),             //
      m_verticesPerSphere(verticesPerShpere),           //
      m_slices(slices), m_stacks(stack)                 //
// drawOrder(std::vector<int>(MaxPolygons)),           //
// sortBuffer(std::make_unique<Vertex[]>(MaxVertices)) //
{};

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

  const uint32_t count = static_cast<uint32_t>(ptr - ptrInit.get());
  vbo.setData(ptrInit.get(), count * sizeof(Vertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  shader.bind();
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

  glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
  const uint32_t color = tintColor.value;
  constexpr float radius = 0.5f;

  // -----------------------------
  // North pole
  // -----------------------------
  {
    glm::vec4 pos{0.0f, radius, 0.0f, 1.f};
    glm::vec2 uv{0.5f, 0.0f};

    ptr->position = glm::vec3(transform * pos);
    ptr->color = color;
    ptr->texCoord = uv;
    ptr->texIndex = textureIndex;
    ptr->tilingFactor = tilingFactor;
    ptr->normal = glm::normalize(normalMatrix * glm::normalize(pos));
    ++ptr;
  }

  // -----------------------------
  // Rings (exclude poles)
  // -----------------------------
  for (uint32_t stack = 1; stack < m_stacks - 1; ++stack) {
    float v = fdiv(stack, m_stacks);
    float phi = glm::pi<float>() * v;

    float y = radius * cos(phi);
    float rTimesSin = radius * sin(phi);

    for (uint32_t slice = 0; slice < m_slices; ++slice) {
      float u = fdiv(slice, m_slices - 1);
      float theta = glm::two_pi<float>() * u;

      float x = rTimesSin * cos(theta);
      float z = rTimesSin * sin(theta);

      glm::vec4 pos{x, y, z, 1.f};
      glm::vec2 uv{u, v};

      ptr->position = glm::vec3(transform * pos);
      ptr->color = color;
      ptr->texCoord = uv;
      ptr->texIndex = textureIndex;
      ptr->tilingFactor = tilingFactor;
      ptr->normal = glm::normalize(normalMatrix * glm::normalize(pos));
      ++ptr;
    }
  }
  // -----------------------------
  // South pole
  // -----------------------------
  {
    glm::vec4 pos{0.0f, -radius, 0.0f, 1.f};
    glm::vec2 uv{0.5f, 1.0f};

    ptr->position = glm::vec3(transform * pos);
    ptr->color = color;
    ptr->texCoord = uv;
    ptr->texIndex = textureIndex;
    ptr->tilingFactor = tilingFactor;
    ptr->normal = glm::normalize(normalMatrix * glm::normalize(pos));
    ++ptr;
  }

  // One sphere added
  indexCount++;

#ifndef NDEBUG
  statsCount++;
#endif
}
} // namespace pain
