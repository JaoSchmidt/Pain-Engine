/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BatchSpray.cpp
#include "CoreRender/Renderer/BatchSpray.h"
#include "ContextBackend.h"
#include "Debugging/Profiling.h"
namespace pain
{

SprayBatch SprayBatch::create()
{
  float vertices[] = {
      -0.5f, 0.5f,  //
      0.5f,  -0.5f, //
      -0.5f, -0.5f, //

      -0.5f, 0.5f,  //
      0.5f,  -0.5f, //
      0.5f,  0.5f,  //
  };
  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  return SprayBatch //
      {*VertexBuffer::createStaticVertexBuffer(
           vertices, sizeof(vertices),
           {
               {ShaderDataType::Float2, "a_Position"},
           }),
       *VertexBuffer::createVertexBuffer(
           MaxPolygons * sizeof(InstanceParticleVertex),
           {
               {ShaderDataType::Float2, "a_Normal", false, true},
               {ShaderDataType::Float, "a_Time", false, true},
               {ShaderDataType::Float2, "a_EmitStart", false, true},
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
      cpuBuffer(std::make_unique<InstanceParticleVertex[]>(MaxVertices)),
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
  if (!instanceCount)
    return;

  vao.bind();

  vbo.bind();

  instanceVBO.bind();
  instanceVBO.setData(cpuBuffer.get(),
                      instanceCount * sizeof(InstanceParticleVertex));

  shader.bind();
  ib.bind();
  backend::drawInstanced(6, instanceCount);
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
