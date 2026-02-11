
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSphere.cpp
#include "CoreRender/Renderer/BatchSphere.h"
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
      *Shader::createFromFile("resources/default/shaders/Texture.glsl");

  int *samplers = new int[backend::getTMU()];
  for (int i = 0; i < backend::getTMU(); ++i)
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
    : m_indicesPerSphere(indicesPerSphere),                                //
      m_verticesPerSphere(verticesPerShpere),                              //
      m_maxIndices(maxIndices),                                            //
      m_slices(slices), m_stacks(stack),                                   //
      vbo(std::move(vbo_)),                                                //
      ib(std::move(ib_)),                                                  //
      vao(*VertexArray::createVertexArray(vbo, ib)),                       //
      shader(std::move(shader_)),                                          //
      ptrInit(std::make_unique<Vertex[]>(maxVertices)), ptr(ptrInit.get()) //
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
// void SphereBatch::allocateSphereUV(const glm::mat4 &transform,
//                                    const Color &tintColor, float
//                                    tilingFactor, float textureIndex)
// {
//   PROFILE_FUNCTION();
//
//   const uint32_t color = tintColor.value;
//   constexpr float radius = 0.5f;
//
//   for (uint32_t stack = 0; stack < m_stacks; ++stack) {
//     float v0 = fdiv(stack, m_stacks);
//     float v1 = fdiv(stack + 1, m_stacks);
//
//     float phi0 = glm::pi<float>() * v0;
//     float phi1 = glm::pi<float>() * v1;
//
//     for (uint32_t slice = 0; slice < m_slices; ++slice) {
//       float u0 = fdiv(slice, m_slices);
//       float u1 = fdiv(slice + 1, m_slices);
//
//       float theta0 = glm::two_pi<float>() * u0;
//       float theta1 = glm::two_pi<float>() * u1;
//
//       // Quad corners (counter-clockwise)
//       glm::vec3 p0{radius * sin(phi0) * cos(theta0), radius * cos(phi0),
//                    radius * sin(phi0) * sin(theta0)};
//
//       glm::vec3 p1{radius * sin(phi1) * cos(theta0), radius * cos(phi1),
//                    radius * sin(phi1) * sin(theta0)};
//
//       glm::vec3 p2{radius * sin(phi1) * cos(theta1), radius * cos(phi1),
//                    radius * sin(phi1) * sin(theta1)};
//
//       glm::vec3 p3{radius * sin(phi0) * cos(theta1), radius * cos(phi0),
//                    radius * sin(phi0) * sin(theta1)};
//
//       glm::vec2 uv0{u0, v0};
//       glm::vec2 uv1{u0, v1};
//       glm::vec2 uv2{u1, v1};
//       glm::vec2 uv3{u1, v0};
//
//       const glm::vec3 positions[4] = {p0, p1, p2, p3};
//       const glm::vec2 uvs[4] = {uv0, uv1, uv2, uv3};
//
//       for (int i = 0; i < 4; i++) {
//         ptr->position = glm::vec3(transform * glm::vec4(positions[i], 1.0f));
//         ptr->color = color;
//         ptr->texCoord = uvs[i];
//         ptr->texIndex = textureIndex;
//         ptr->tilingFactor = tilingFactor;
//         ptr++;
//       }
//     }
//   }
//   indexCount++;
// #ifndef NDEBUG
//   statsCount++;
// #endif
// }
float inline fdiv(uint32_t divided, uint32_t divisor)
{
  return static_cast<float>(divided) / static_cast<float>(divisor);
}
void SphereBatch::allocateSphereUV(const glm::mat4 &transform,
                                   const Color &tintColor, float tilingFactor,
                                   float textureIndex)
{
  PROFILE_FUNCTION();

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
    ++ptr;
  }

  // One sphere added
  indexCount++;

#ifndef NDEBUG
  statsCount++;
#endif
}
} // namespace pain
