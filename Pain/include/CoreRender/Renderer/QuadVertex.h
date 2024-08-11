#pragma once
#include "pch.h"

#include "Core.h"

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

class QuadVertexBatch
{
  QuadVertexBatch();
  friend class Renderer2d;
  const std::shared_ptr<VertexArray> &getVertexArray() const
  {
    return m_vertexArray;
  };
  const uint32_t &getIndexCount() const { return m_indexCount; };
  void goBackToFirstQuad();
  void sendAllDataToOpenGL();
  inline const std::shared_ptr<Shader> getShader() const
  {
    return m_textureShader;
  };
  void bindTextures();
  // clang-format on

  // Quads
  const uint32_t MaxQuads = 10000;
  const uint32_t MaxVertices = MaxQuads * 4;
  const uint32_t MaxIndices = MaxQuads * 6;

  const uint32_t MaxTri = 10000;
  const uint32_t MaxTriVertices = MaxTri * 3;

  // TODO: eventually dinamically see MaxTextureSlots value on gpu
  static const uint32_t MaxTextureSlots = 32;

  std::shared_ptr<VertexArray> m_vertexArray;
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<Shader> m_textureShader;
  std::shared_ptr<Texture> m_whiteTexture;
  std::array<std::shared_ptr<Texture>, MaxTextureSlots> m_textureSlots;

  QuadVertex *m_vertexBufferBase = nullptr;
  QuadVertex *m_vertexBufferPtr = nullptr;
  uint32_t m_indexCount = 0;
  uint32_t m_textureSlotIndex = 1;
  // clang-format off
  constexpr static glm::vec4 m_quadVertexPositions[4] = {
      glm::vec4(-0.5f,-0.5f,0.f,1.f),
      glm::vec4( 0.5f,-0.5f,0.f,1.f),
      glm::vec4( 0.5f, 0.5f,0.f,1.f),
      glm::vec4(-0.5f, 0.5f,0.f,1.f),
  };
  // clang format on


  // ================================================================= //
  // Draws
  // ================================================================= //

  // clang-format off
  void drawQuad(
    const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
    const std::shared_ptr<Texture> &texture,
    const float tilingFactor,
    const std::array<glm::vec2, 4> &textureCoordinate);

  void drawQuad(
    const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
    const float textureIndex, // default texture index = 0.0f
    const float tilingFactor,
    const std::array<glm::vec2, 4> &textureCoordinate);

  void drawRotQuad(
    const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
    const float textureIndex, // default texture index = 0.0f
    const float tilingFactor,
    const std::array<glm::vec2, 4> &textureCoordinate,
    const float rotation);

  void drawRotQuad(
    const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
    const std::shared_ptr<Texture> &texture,
    const float tilingFactor,
    const std::array<glm::vec2, 4> &textureCoordinate,
    const float rotation);

  // void drawTri(
  //   const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
  //   const std::shared_ptr<Texture> &texture,
  //   const float tilingFactor,
  //   const std::array<glm::vec2, 4> &textureCoordinate);

  // void drawTri(
  //   const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
  //   const float textureIndex, // default texture index = 0.0f
  //   const float tilingFactor,
  //   const std::array<glm::vec2, 4> &textureCoordinate);

  // void drawRotTri(
  //   const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
  //   const float textureIndex, // default texture index = 0.0f
  //   const float tilingFactor,
  //   const std::array<glm::vec2, 4> &textureCoordinate,
  //   const float rotation);

  // void drawRotTri(
  //   const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &tintColor,
  //   const std::shared_ptr<Texture> &texture,
  //   const float tilingFactor,
  //   const std::array<glm::vec2, 4> &textureCoordinate,
  //   const float rotation);

 };

} // namespace pain
