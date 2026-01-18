/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// MiscDebugGrid.h
#pragma once

#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
namespace pain
{

struct GridVertex {
  glm::vec3 position;
};

struct DebugGrid {
  using Vertex = GridVertex;

  VertexBuffer vbo;
  IndexBuffer ib;
  VertexArray vao;
  Shader shader;
  bool enableGrid = true;

  static DebugGrid create(float gridSize = 0.5f);
  void flush();

private:
  DebugGrid(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_);
};
} // namespace pain
