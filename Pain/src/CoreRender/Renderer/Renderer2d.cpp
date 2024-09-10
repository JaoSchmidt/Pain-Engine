#include "CoreRender/Renderer/Renderer2d.h"
#include "Misc/BasicOrthoCamera.h"

namespace pain
{
std::shared_ptr<OrthoCameraEntity> Renderer2d::m_cameraEntity = nullptr;
// OrthoCameraEntity *Renderer2d::m_cameraEntity = nullptr;

// ================================================================= //
// Render initialization and destruction
// ================================================================= //

void Renderer2d::init(std::shared_ptr<OrthoCameraEntity> cameraEntity)
{
  P_ASSERT(cameraEntity != nullptr, "Camera Entity must be initialized");
  // NOTE: This can be changed later in case we need some camera mechanic
  // glEnable(GL_DEPTH_TEST);

  // transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // quadBatch = new VertexBatch();
  initBatches();
  m_cameraEntity = cameraEntity;
}

// ================================================================= //
// Renderer: basic wrapper around opengl
// ================================================================= //
void Renderer2d::shutdown() {}

void Renderer2d::setViewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}

void Renderer2d::clear() { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer2d::setClearColor(const glm::vec4 &color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer2d::beginScene(float globalTime, const glm::mat4 &transform)
{
  m_quadTextureShader->bind();
  m_quadTextureShader->uploadUniformMat4(
      "u_ViewProjection", m_cameraEntity->getComponent<OrthoCameraComponent>()
                              .m_camera->getViewProjectionMatrix());
  m_quadTextureShader->uploadUniformMat4("u_Transform", transform);

  m_triShader->bind();
  m_triShader->uploadUniformMat4(
      "u_ViewProjection", m_cameraEntity->getComponent<OrthoCameraComponent>()
                              .m_camera->getViewProjectionMatrix());
  m_triShader->uploadUniformMat4("u_Transform", transform);

  m_sprayShader->bind();
  m_sprayShader->uploadUniformMat4(
      "u_ViewProjection", m_cameraEntity->getComponent<OrthoCameraComponent>()
                              .m_camera->getViewProjectionMatrix());
  m_sprayShader->uploadUniformMat4("u_Transform", transform);
  m_sprayShader->uploadUniformFloat("u_Time", globalTime);

  goBackToFirstVertex();
}

void Renderer2d::endScene()
{
  // quadBatch->sendAllDataToOpenGL();
  // NOTE: sendAllDataToOpenGL probably won't be here in the future,
  // otherwise flush() wouldn't need to be a function
  flush();
}

void Renderer2d::flush()
{
  // bindTextures();
  drawBatches(m_cameraEntity->getComponent<OrthoCameraComponent>()
                  .m_camera->getViewProjectionMatrix());
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

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const std::shared_ptr<Texture> &texture,
                          float tilingFactor, // will prevent fancy tiling
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex =
      texture ? allocateTextures(texture) : 0.0f; // White Texture if nullptr
  const glm::mat4 transform = getTransform(position, size);
  allocateQuad(transform, tintColor, tilingFactor, texIndex, textureCoordinate);
}

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const float rotationRadians,
                          const std::shared_ptr<Texture> &texture,
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex =
      texture ? allocateTextures(texture) : 0.0f; // White Texture if nullptr
  const glm::mat4 transform = getTransform(position, size, rotationRadians);
  allocateQuad(transform, tintColor, tilingFactor, texIndex, textureCoordinate);
}

// ================================================================= //
// Draw Tri
// ================================================================= //

void Renderer2d::drawTri(const glm::vec2 &position, const glm::vec2 &size,
                         const glm::vec4 &tintColor)
{
  const glm::mat4 transform = getTransform(position, size);
  allocateTri(transform, tintColor);
}
void Renderer2d::drawTri(const glm::vec2 &position, const glm::vec2 &size,
                         const glm::vec4 &tintColor,
                         const float rotationRadians)
{
  const glm::mat4 transform = getTransform(position, size, rotationRadians);
  allocateTri(transform, tintColor);
}

// ================================================================= //
// Draw Spray Particles
// ================================================================= //

void Renderer2d::drawSprayParticle(const Particle &p)
{
  allocateSprayParticles(p.m_position, p.m_offset, p.m_normal, p.m_startTime,
                         p.m_rotationSpeed);
}

} // namespace pain
