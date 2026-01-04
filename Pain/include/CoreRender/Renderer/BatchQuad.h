// QuadBatch.h
#pragma once

#include "ContextBackend.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
namespace pain
{

struct QuadVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 texCoord;
  float texIndex;
  float tilingFactor;
};

// TODO:(jao) search MaxTextureSlots dinamically (i.e TMU value on gpu)
static constexpr uint32_t MaxTextureSlots = 32;

struct QuadBatch {
  using Vertex = QuadVertex;
  static constexpr uint32_t MaxPolygons = 40000;
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

  static QuadBatch create();

  void allocateQuad(const glm::mat4 &transform, const glm::vec4 &tintColor,
                    const float tilingFactor, const float textureIndex,
                    const std::array<glm::vec2, 4> &textureCoordinate);
  void resetAll();
  void resetPtr();
  void flush(const std::array<Texture *, MaxTextureSlots> &textures,
             uint32_t textureCount);

private:
  QuadBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_);
};
} // namespace pain
