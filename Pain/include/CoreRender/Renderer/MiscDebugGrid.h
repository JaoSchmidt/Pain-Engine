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
  bool enableGrid = false;

  static DebugGrid create(float gridSize = 2.f);
  void flush();

private:
  DebugGrid(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_);
};
} // namespace pain
