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

struct TriVertex {
  glm::vec3 position;
  glm::vec4 color;
};

class VertexBatch
{
  VertexBatch();
  friend class Renderer2d;
  const std::shared_ptr<VertexArray> &getQuadVertexArray() const
  {
    return m_quadVertexArray;
  };
  const std::shared_ptr<VertexArray> &getTriVertexArray() const
  {
    return m_triVertexArray;
  };
  const uint32_t &getQuadIndexCount() const { return m_quadIndexCount; };
  void goBackToFirstVertex();
  void drawBatch(const glm::mat4 &viewProjectionMatrix);
  void sendAllDataToOpenGL();
  inline const std::shared_ptr<Shader> getQuadShader() const
  {
    return m_quadTextureShader;
  };
  inline const std::shared_ptr<Shader> getTriShader() const
  {
    return m_triShader;
  };
  void bindTextures();
  // clang-format on

  constexpr static uint32_t MaxQuads = 10000;
  constexpr static uint32_t MaxQuadVertices = MaxQuads * 4;
  constexpr static uint32_t MaxQuadIndices = MaxQuads * 6;

  constexpr static uint32_t MaxTri = 10000;
  constexpr static uint32_t MaxTriVertices = MaxTri * 3;
  constexpr static uint32_t MaxTriIndices = MaxTri * 3;

  // TODO: eventually dinamically see MaxTextureSlots value on gpu
  static const uint32_t MaxTextureSlots = 32;

  std::shared_ptr<VertexArray> m_quadVertexArray;
  std::shared_ptr<VertexBuffer> m_quadVertexBuffer;
  std::shared_ptr<Shader> m_quadTextureShader;

  std::shared_ptr<VertexArray> m_triVertexArray;
  std::shared_ptr<VertexBuffer> m_triVertexBuffer;
  std::shared_ptr<Shader> m_triShader;

  std::shared_ptr<Texture> m_whiteTexture;
  std::array<std::shared_ptr<Texture>, MaxTextureSlots> m_textureSlots;

  QuadVertex *m_quadVertexBufferBase = nullptr;
  QuadVertex *m_quadVertexBufferPtr = nullptr;
  TriVertex *m_triVertexBufferBase = nullptr;
  TriVertex *m_triVertexBufferPtr = nullptr;
  uint32_t m_quadIndexCount = 0;
  uint32_t m_triIndexCount = 0;
  uint32_t m_textureSlotIndex = 1;
  // clang-format off
  constexpr static glm::vec4 m_quadVertexPositions[4] = {
      glm::vec4(-0.5f,-0.5f,0.f,1.f),
      glm::vec4( 0.5f,-0.5f,0.f,1.f),
      glm::vec4( 0.5f, 0.5f,0.f,1.f),
      glm::vec4(-0.5f, 0.5f,0.f,1.f),
  };
  constexpr static glm::vec4 m_triVertexPositions[3] = {
      glm::vec4( 0.0f, 0.5f,0.f,1.f),
      glm::vec4( 0.5f,-0.5f,0.f,1.f),
      glm::vec4(-0.5f,-0.5f,0.f,1.f),
  };


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

  // ===== Triangles =========================================== //
  void drawTri(const glm::vec2 &position, const glm::vec2 &size,
               const glm::vec4 &tintColor);

  void drawRotTri(const glm::vec2 &position, const glm::vec2 &size,
                  const glm::vec4 &tintColor, const float rotation);
};

} // namespace pain
