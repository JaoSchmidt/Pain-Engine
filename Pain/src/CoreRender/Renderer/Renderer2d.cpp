#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/QuadVertex.h"
#include "Misc/BasicOrthoCamera.h"

namespace pain
{
static QuadVertexBatch *quadBatch = nullptr;
std::shared_ptr<OrthoCameraEntity> Renderer2d::m_cameraController = nullptr;

// ================================================================= //
// Render initialization and destruction
// ================================================================= //

void Renderer2d::init(
    const std::shared_ptr<OrthoCameraEntity> &cameraController)
{
  quadBatch = new QuadVertexBatch();
  // NOTE: This can be changed later in case the engine needs a camera mechanic
  m_cameraController = cameraController;
}

void Renderer2d::shutdown() { delete quadBatch; }

// ================================================================= //
// Draws
// ================================================================= //

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &color)
{
  quadBatch->drawQuad(position, size, color);
}

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const std::shared_ptr<Texture> &texture,
                          float tilingFactor, glm::vec4 tintColor)
{
  quadBatch->drawQuad(position, size, texture, tilingFactor, tintColor);
}

// ================================================================= //
// Renderer basic wrapper around opengl
// ================================================================= //

void Renderer2d::setViewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}

void Renderer2d::clear() { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer2d::setClearColor(const glm::vec4 color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer2d::beginScene(const glm::mat4 &transform)
{
  quadBatch->getShader()->bind();
  quadBatch->getShader()->uploadUniformMat4(
      "u_ViewProjection",
      m_cameraController->getCamera().getViewProjectionMatrix());
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

} // namespace pain
