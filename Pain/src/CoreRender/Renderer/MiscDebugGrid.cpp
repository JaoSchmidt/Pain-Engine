/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/MiscDebugGrid.h"
#include "ContextBackend.h"

namespace pain
{

DebugGrid DebugGrid::create(float gridSize)
{
  float vertices[] = {
      -1.0f, -1.0f, 0.0f, //
      1.0f,  -1.0f, 0.0f, //
      1.0f,  1.0f,  0.0f, //
      -1.0f, 1.0f,  0.0f,
  }; //

  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  Shader shader =
      *Shader::createFromFile("resources/default/shaders/InfiniteGrid.glsl");
  shader.bind();
  shader.uploadUniformFloat3("u_Color", glm::vec3(0.1f, 0.6f, 0.9f));
  shader.uploadUniformFloat("u_CellSize", gridSize);
  shader.uploadUniformFloat("u_Thickness", 0.005f);
  return DebugGrid(
      *VertexBuffer::createStaticVertexBuffer(
          vertices, sizeof(vertices), {{ShaderDataType::Float3, "a_Position"}}),
      *IndexBuffer::createIndexBuffer(indices,
                                      sizeof(indices) / sizeof(indices[0])),
      std::move(shader));
}
DebugGrid::DebugGrid(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)),
      shader(std::move(shader_)) {};

void DebugGrid::flush()
{
  if (!enableGrid)
    return;

  vao.bind();
  vbo.bind();
  shader.bind();
  // ib.bind();

  backend::drawIndexed(vao, 6);
}
} // namespace pain
