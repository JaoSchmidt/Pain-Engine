/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSpray.cpp
#include "CoreRender/Renderer/BatchSpray.h"
#include "Debugging/Profiling.h"
#include "platform/ContextBackend.h"
#
namespace pain
{

SprayBatch SprayBatch::create()
{
  constexpr unsigned int indices[] = {0, 1, 2, 2, 3, 0};
  // constexpr float vertices[] = {
  //     -0.5f, 0.5f,  // 0 top left
  //     -0.5f, -0.5f, // 1 bottom left
  //     0.5f,  -0.5f, // 2 bottom right
  //
  //     0.5f,  -0.5f, // 2 bottom right
  //     0.5f,  0.5f,  // 3 top right
  //     -0.5f, 0.5f,  // 0 top left
  // };
  constexpr glm::vec2 SprayVertexPositions[4] = {
      glm::vec2(-0.5F, -0.5F),
      glm::vec2(0.5F, -0.5F),
      glm::vec2(0.5F, 0.5F),
      glm::vec2(-0.5F, 0.5F),
  };

  std::unique_ptr<ParticleVertex[]> vertices =
      std::make_unique<ParticleVertex[]>(VerticesPerParticle);
  ParticleVertex *pVertex = vertices.get();

  for (uint32_t i = 0; i < VerticesPerParticle; i++) {
    pVertex->position = SprayVertexPositions[i];
    pVertex++;
  }

  return SprayBatch //
      {*VertexBuffer::createStaticVertexBuffer(
           vertices.get(), sizeof(ParticleVertex) * VerticesPerParticle,
           {
               {ShaderDataType::Float2, "a_Position"},
           }),
       *VertexBuffer::createVertexBuffer(
           MaxPolygons * sizeof(InstanceParticleVertex),
           {
               {ShaderDataType::Float2, "a_Normal", false, true},
               {ShaderDataType::Float, "a_Time", false, true},
               {ShaderDataType::Float2, "a_EmitStart", false, true},
               {ShaderDataType::Mat4, "a_Transform", false, true},
           }),
       *IndexBuffer::createIndexBuffer(indices,
                                       sizeof(indices) / sizeof(indices[0])),
       *Shader::createFromFile(
           "resources/default/shaders/SprayParticles.glsl")};
}
SprayBatch::SprayBatch(VertexBuffer &&vbo_, VertexBuffer &&vboInstance_,
                       IndexBuffer &&ib_, Shader &&shader)
    : vbo(std::move(vbo_)), instanceVBO(std::move(vboInstance_)),
      ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, instanceVBO, ib)),
      shader(std::move(shader)),
      cpuBuffer(std::make_unique<InstanceParticleVertex[]>(MaxPolygons)),
      ptr(cpuBuffer.get()) {};

void SprayBatch::resetPtr()
{
  instanceCount = 0;
  ptr = cpuBuffer.get();
}
void SprayBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void SprayBatch::flush()
{
  if (instanceCount == 0)
    return;

  vao.bind();

  vbo.bind();

  instanceVBO.bind();
  instanceVBO.setData(cpuBuffer.get(),
                      instanceCount * sizeof(InstanceParticleVertex));

  shader.bind();
  ib.bind();
  backend::drawIndexedInstanced(vao, 6, instanceCount);
#ifndef NDEBUG
  drawCount++;
#endif
}

void SprayBatch::allocateSprayParticles(const glm::vec2 &normal,
                                        const DeltaTime startTime,
                                        const glm::vec2 &emitStart)
{
  PROFILE_FUNCTION();
  ptr->normal = normal;
  ptr->startTime = startTime.getSecondsf();
  ptr->emitStart = emitStart;
  ptr++;
  instanceCount++;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
