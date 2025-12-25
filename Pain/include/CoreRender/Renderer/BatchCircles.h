// CircleBatch.h
#pragma once
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

struct CircleVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 coord;
};
struct CircleBatch {
  using Vertex = CircleVertex;
  static constexpr uint32_t MaxPolygons = 1000;
  static constexpr uint32_t MaxVertices = MaxPolygons * 4;
  static constexpr uint32_t MaxIndices = MaxPolygons * 6;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  IndexBuffer ib;
  VertexArray vao;
  Shader shader;

  std::unique_ptr<Vertex[]> cpuBuffer;
  Vertex *ptr = nullptr;
  uint32_t indexCount = 0;

  static CircleBatch create();
  void resetAll();
  void resetPtr();
  void flush();

  void allocateCircle(const glm::mat4 &transform, const glm::vec4 &tintColor,
                      const std::array<glm::vec2, 4> &coordinate);

private:
  CircleBatch(VertexBuffer &&vbo, IndexBuffer &&ib, Shader &&shader);
};

} // namespace pain
