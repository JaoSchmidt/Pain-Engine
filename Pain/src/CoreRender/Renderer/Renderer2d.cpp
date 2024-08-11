#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/QuadVertex.h"
#include "Misc/BasicOrthoCamera.h"

namespace pain
{
static QuadVertexBatch *quadBatch = nullptr;
std::shared_ptr<OrthoCameraEntity> Renderer2d::m_cameraEntity = nullptr;
// OrthoCameraEntity *Renderer2d::m_cameraEntity = nullptr;

// ================================================================= //
// Render initialization and destruction
// ================================================================= //

void Renderer2d::init(std::shared_ptr<OrthoCameraEntity> cameraEntity)
{
  P_ASSERT(cameraEntity != nullptr, "Camera Entity must be initialized");
  quadBatch = new QuadVertexBatch();
  // NOTE: This can be changed later in case the engine needs a camera mechanic

  // glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  m_cameraEntity = cameraEntity;
}

// ================================================================= //
// Renderer: basic wrapper around opengl
// ================================================================= //
void Renderer2d::shutdown() { delete quadBatch; }

void Renderer2d::setViewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}

void Renderer2d::clear() { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer2d::setClearColor(const glm::vec4 &color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer2d::beginScene(const glm::mat4 &transform)
{
  quadBatch->getShader()->bind();
  quadBatch->getShader()->uploadUniformMat4(
      "u_ViewProjection", m_cameraEntity->getComponent<OrthoCameraComponent>()
                              .m_camera->getViewProjectionMatrix());
  quadBatch->getShader()->uploadUniformMat4("u_Transform", transform);

  quadBatch->goBackToFirstQuad();
}

void Renderer2d::endScene()
{
  quadBatch->sendAllDataToOpenGL();
  // NOTE: sendAllDataToOpenGL probably won't be here in the future,
  // otherwise flush() wouldn't need to be a function
  flush();
}

void Renderer2d::flush()
{
  quadBatch->bindTextures();
  drawIndexed(quadBatch->getVertexArray(), quadBatch->getIndexCount());
}

void Renderer2d::drawIndexed(const std::shared_ptr<VertexArray> &vertexArray,
                             uint32_t indexCount)
{
  uint32_t count =
      indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
}

// ================================================================= //
// Draw Quads
// ================================================================= //

// Draw texturesless quad
void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor)
{
  constexpr float texIndex = 0.0f; // White Texture
  constexpr float tilingFactor = 1.0f;
  constexpr std::array<glm::vec2, 4> textureCoordinate = {
      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(0.0f, 1.0f)};
  quadBatch->drawQuad(position, size, tintColor, tilingFactor, texIndex,
                      textureCoordinate);
}

// Draw quad w/texture
void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const std::shared_ptr<Texture> &texture,
                          float tilingFactor)
{
  constexpr std::array<glm::vec2, 4> textureCoordinate = {
      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(0.0f, 1.0f)};
  quadBatch->drawQuad(position, size, tintColor, texture, tilingFactor,
                      textureCoordinate);
}

// Draw quad w/texture and its specific coordinates (e.g. for spritesheets)
void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const std::shared_ptr<Texture> &texture,
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  quadBatch->drawQuad(position, size, tintColor, texture, tilingFactor,
                      textureCoordinate);
}

// ===== Draw w/rotation =========================================== //

void Renderer2d::drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                             const glm::vec4 &tintColor, const float rotation)
{
  constexpr float texIndex = 0.0f; // White Texture
  constexpr float tilingFactor = 1.0f;
  constexpr std::array<glm::vec2, 4> textureCoordinate = {
      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(0.0f, 1.0f)};
  quadBatch->drawRotQuad(position, size, tintColor, tilingFactor, texIndex,
                         textureCoordinate, rotation);
}
// Draw quad w/rotation and texture
void Renderer2d::drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                             const glm::vec4 &tintColor,
                             const std::shared_ptr<Texture> &texture,
                             const float tilingFactor, const float rotation)
{
  constexpr std::array<glm::vec2, 4> textureCoordinate = {
      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(0.0f, 1.0f)};
  quadBatch->drawRotQuad(position, size, tintColor, texture, tilingFactor,
                         textureCoordinate, rotation);
}

// Draw quad w/rotation, texture and its specific coordinates (e.g. for
// spritesheets)
void Renderer2d::drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                             const glm::vec4 &tintColor,
                             const std::shared_ptr<Texture> &texture,
                             const float tilingFactor,
                             const std::array<glm::vec2, 4> &textureCoordinate,
                             const float rotation)
{
  quadBatch->drawRotQuad(position, size, tintColor, texture, tilingFactor,
                         textureCoordinate, rotation);
}

// ================================================================= //
// Draw Tri
// ================================================================= //

// void Renderer2d::drawTri(const glm::vec2 &position, const glm::vec2 &size,
//                          const glm::vec4 &tintColor)
// {
//   constexpr float texIndex = 0.0f; // White Texture
//   constexpr float tilingFactor = 1.0f;
//   constexpr std::array<glm::vec2, 4> textureCoordinate = {
//       glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
//       glm::vec2(0.0f, 1.0f)};
//   quadBatch->drawTri(position, size, tintColor, tilingFactor, texIndex,
//                      textureCoordinate);
// }
// // Draw quad w/rotation and texture
// void Renderer2d::drawTri(const glm::vec2 &position, const glm::vec2 &size,
//                          const glm::vec4 &tintColor,
//                          const std::shared_ptr<Texture> &texture,
//                          const float tilingFactor)
// {
//   constexpr std::array<glm::vec2, 4> textureCoordinate = {
//       glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
//       glm::vec2(0.0f, 1.0f)};
//   quadBatch->drawTri(position, size, tintColor, texture, tilingFactor,
//                      textureCoordinate);
// }

// // Draw quad w/rotation, texture and its specific coordinates (e.g. for
// // spritesheets)
// void Renderer2d::drawTri(const glm::vec2 &position, const glm::vec2 &size,
//                          const glm::vec4 &tintColor,
//                          const std::shared_ptr<Texture> &texture,
//                          const float tilingFactor,
//                          const std::array<glm::vec2, 4> &textureCoordinate)
// {
//   quadBatch->drawTri(position, size, tintColor, texture, tilingFactor,
//                      textureCoordinate);
// }
// // ===== Draw w/rotation =========================================== //
// void Renderer2d::drawRotTri(const glm::vec2 &position, const glm::vec2 &size,
//                             const glm::vec4 &tintColor, const float rotation)
// {
//   constexpr float texIndex = 0.0f; // White Texture
//   constexpr float tilingFactor = 1.0f;
//   constexpr std::array<glm::vec2, 4> textureCoordinate = {
//       glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
//       glm::vec2(0.0f, 1.0f)};
//   quadBatch->drawRotTri(position, size, tintColor, tilingFactor, texIndex,
//                         textureCoordinate, rotation);
// }
// // Draw quad w/rotation and texture
// void Renderer2d::drawRotTri(const glm::vec2 &position, const glm::vec2 &size,
//                             const glm::vec4 &tintColor,
//                             const std::shared_ptr<Texture> &texture,
//                             const float tilingFactor, const float rotation)
// {
//   constexpr std::array<glm::vec2, 4> textureCoordinate = {
//       glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
//       glm::vec2(0.0f, 1.0f)};
//   quadBatch->drawRotTri(position, size, tintColor, texture, tilingFactor,
//                         textureCoordinate, rotation);
// }
// // Draw quad w/rotation, texture and its specific coordinates (e.g. for
// // spritesheets)
// void Renderer2d::drawRotTri(const glm::vec2 &position, const glm::vec2 &size,
//                             const glm::vec4 &tintColor,
//                             const std::shared_ptr<Texture> &texture,
//                             const float tilingFactor,
//                             const std::array<glm::vec2, 4>
//                             &textureCoordinate, const float rotation)
// {
//   quadBatch->drawRotTri(position, size, tintColor, texture, tilingFactor,
//                         textureCoordinate, rotation);
// }

} // namespace pain
