#pragma once

#include "Core.h"

#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "Misc/BasicOrthoCamera.h"

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

class EXPORT Renderer2d
{
public:
  static void init(std::shared_ptr<OrthoCameraEntity> cameraEntity);
  // ================================================================= //
  // Renderer basic wrapper around opengl
  // ================================================================= //

  static void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);
  static void beginScene(const glm::mat4 &transform = glm::mat4(1.0f));
  static void endScene();
  static void shutdown();
  static void setViewport(int x, int y, int width, int height);
  static void setClearColor(const glm::vec4 &color);
  static void clear();
  static void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray,
                          uint32_t indexCount = 0);

  // static OrthoCameraEntity *m_cameraEntity;
  static std::shared_ptr<OrthoCameraEntity> m_cameraEntity;

  // ================================================================= //
  // Draw Quads
  // ================================================================= //

  /** Draws a quad */
  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const glm::vec4 &tintColor,
                       const std::shared_ptr<Texture> &texture = nullptr,
                       const float tilingFactor = 1.0f,
                       const std::array<glm::vec2, 4> &textureCoordinate = {
                           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
  /** Draws a quad with rotation, new rotationRadians (angle)*/
  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const glm::vec4 &tintColor, const float rotationRadians,
                       const std::shared_ptr<Texture> &texture = nullptr,
                       const float tilingFactor = 1.0f,
                       const std::array<glm::vec2, 4> &textureCoordinate = {
                           glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                           glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

  // ================================================================= //
  // Draw Tri
  // ================================================================= //

  static void drawTri(const glm::vec2 &position, const glm::vec2 &size,
                      const glm::vec4 &tintColor);

  /** Draws triangles with rotation, new rotationRadians (angle)*/
  static void drawTri(const glm::vec2 &position, const glm::vec2 &size,
                      const glm::vec4 &tintColor, const float rotationRadians);

  static const glm::mat4 getTransform(const glm::vec2 &position,
                                      const glm::vec2 &size,
                                      const float rotationRadians);
  static const glm::mat4 getTransform(const glm::vec2 &position,
                                      const glm::vec2 &size);

private:
  static void flush();
  // ================================================================= //
  // Data for Renderer
  // ================================================================= //
  static void initBatches();
  static void bindTextures();
  static float allocateTextures(const std::shared_ptr<Texture> &texture);

  static void allocateQuad(const glm::mat4 &transform,
                           const glm::vec4 &tintColor, const float tilingFactor,
                           const float textureIndex,
                           const std::array<glm::vec2, 4> &textureCoordinate);
  static void allocateTri(const glm::mat4 &transform,
                          const glm::vec4 &tintColor);

  static void goBackToFirstVertex();
  static void drawBatches(const glm::mat4 &viewProjectionMatrix);
  static void sendAllDataToOpenGL();

  constexpr static uint32_t MaxQuads = 10000;
  constexpr static uint32_t MaxQuadVertices = MaxQuads * 4;
  constexpr static uint32_t MaxQuadIndices = MaxQuads * 6;

  constexpr static uint32_t MaxTri = 10000;
  constexpr static uint32_t MaxTriVertices = MaxTri * 3;
  constexpr static uint32_t MaxTriIndices = MaxTri * 3;

  // TODO: search MaxTextureSlots dinamically (i.e TMU value on gpu)
  static const uint32_t MaxTextureSlots = 32;

  static std::shared_ptr<VertexArray> m_quadVertexArray;
  static std::shared_ptr<VertexBuffer> m_quadVertexBuffer;
  static std::shared_ptr<Shader> m_quadTextureShader;

  static std::shared_ptr<VertexArray> m_triVertexArray;
  static std::shared_ptr<VertexBuffer> m_triVertexBuffer;
  static std::shared_ptr<Shader> m_triShader;

  static std::shared_ptr<Texture> m_whiteTexture;
  static std::array<std::shared_ptr<Texture>, MaxTextureSlots> m_textureSlots;

  static QuadVertex *m_quadVertexBufferBase;
  static QuadVertex *m_quadVertexBufferPtr;
  static TriVertex *m_triVertexBufferBase;
  static TriVertex *m_triVertexBufferPtr;
  static uint32_t m_quadIndexCount;
  static uint32_t m_triIndexCount;
  static uint32_t m_textureSlotIndex;
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

};

} // namespace pain
