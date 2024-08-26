#pragma once

#include "Core.h"

#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "Misc/BasicOrthoCamera.h"

namespace pain
{

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
  // Draw texturesless quad
  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const glm::vec4 &tintColor);
  // Draw quad w/texture
  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const glm::vec4 &tintColor,
                       const std::shared_ptr<Texture> &texture,
                       const float tilingFactor);
  // Draw quad w/texture and its specific coordinates (e.g. for spritesheets)
  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const glm::vec4 &tintColor,
                       const std::shared_ptr<Texture> &texture,
                       const float tilingFactor,
                       const std::array<glm::vec2, 4> &textureCoordinate);
  // ===== Draw w/rotation =========================================== //
  static void drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const float rotationRadians);
  // Draw quad w/texture
  static void drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const std::shared_ptr<Texture> &texture,
                          const float tilingFactor,
                          const float rotationRadians);
  // Draw quad w/texture and its specific coordinates (e.g. for spritesheets)
  static void drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const std::shared_ptr<Texture> &texture,
                          const float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate,
                          const float rotationRadians);

  // ================================================================= //
  // Draw Tri
  // ================================================================= //
  static void drawTri(const glm::vec2 &position, const glm::vec2 &size,
                      const glm::vec4 &tintColor);
  static void drawRotTri(const glm::vec2 &position, const glm::vec2 &size,
                         const glm::vec4 &tintColor,
                         const float rotationRadians);

private:
  static void flush();
};

} // namespace pain
