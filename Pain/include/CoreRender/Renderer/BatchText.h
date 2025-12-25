// TextBatch.h
#pragma once
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

struct TextQuadVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 texCoord;
  // NOTE: (jao) you might want to implement `int texIndex` here but one texture
  // is more than fine for now
  // NOTE: thinking better, you are probably use one texture per font, so
  // texIndex will need to be implemented in the future (for implementing
  // multiple fonts)
};

struct TextBatch {
  using Vertex = TextQuadVertex;
  static constexpr uint32_t MaxGlyphs = 40000;
  static constexpr uint32_t MaxVertices = MaxGlyphs * 4;
  static constexpr uint32_t MaxIndices = MaxGlyphs * 6;
  uint32_t statsCount = 0;
  uint32_t drawCount = 0;

  VertexBuffer vbo;
  IndexBuffer ib;
  VertexArray vao;
  Shader shader;

  std::unique_ptr<Vertex[]> cpuBuffer;
  Vertex *ptr = nullptr;
  uint32_t indexCount = 0;

  const Texture *fontAtlas = nullptr;

  static TextBatch create();
  void resetAll();
  void resetPtr();
  void flush();

  void allocateCharacter(const glm::mat4 &transform, const glm::vec4 &tintColor,
                         const std::array<glm::vec2, 4> &textureCoordinate,
                         const std::array<glm::vec4, 4> &textVertexPositions);

private:
  TextBatch(VertexBuffer &&vbo, IndexBuffer &&ib, Shader &&shader);
};

} // namespace pain
