#include "CoreRender/Renderer/Renderer2d.h"
#include "Misc/BasicOrthoCamera.h"

#include "glm/ext/matrix_transform.hpp"

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

  m_textTextureShader->bind();
  m_textTextureShader->uploadUniformMat4(
      "u_ViewProjection", m_cameraEntity->getComponent<OrthoCameraComponent>()
                              .m_camera->getViewProjectionMatrix());
  m_textTextureShader->uploadUniformMat4("u_Transform", transform);

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
                          const Texture *texture, // Raw pointer version
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex =
      texture ? allocateTextures(*texture) : 0.0f; // White texture if nullptr
  const glm::mat4 transform = getTransform(position, size);
  allocateQuad(transform, tintColor, tilingFactor, texIndex, textureCoordinate);
}

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const Texture &texture, // Reference version
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size);
  allocateQuad(transform, tintColor, tilingFactor, texIndex, textureCoordinate);
}

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const float rotationRadians, const Texture &texture,
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size, rotationRadians);
  allocateQuad(transform, tintColor, tilingFactor, texIndex, textureCoordinate);
}

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor,
                          const float rotationRadians, const Texture *texture,
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex =
      texture ? allocateTextures(*texture) : 0.0f; // White Texture if nullptr
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

// ================================================================= //
// Draw Text
// ================================================================= //
void Renderer2d::drawString(const glm::vec2 &position, const char *string,
                            const Font &font, const glm::vec4 &color)
{

  const auto &fontGeometry = font.getFontGeometry();
  const auto &metrics = fontGeometry.getMetrics();
  const double &spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();
  m_fontAtlasTexture = &font.getAtlasTexture();

  double x = 0.0;
  double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
  double y = 0.0;
  float lineHeightOffset = 0.0f;
  for (const char *t = string; *t != '\0'; t++) {
    switch (*t) {
    case '\r':
      continue;
      break;
    case '\n':
      x = 0;
      y -= fsScale * metrics.lineHeight + lineHeightOffset;
      continue;
      break;
    case '\t':
      // https://stackoverflow.com/questions/55594248/how-to-determine-tab-widthin-pixels-for-non-monospace-font
      x += 4.0f * (fsScale * spaceGlyphAdvance);
      break;
    default:
      auto glyph = fontGeometry.getGlyph(*t);
      if (!glyph) {
        drawQuad({-0.5f, 0.0f}, {0.3f, 0.3f}, {1.f, 0.f, 0.f, 1.f});
        PLOG_E("Glyph '{}' not available on font family", *t);
      }

      double atlasLeft, atlasBottom, atlasRight, atlasTop;
      glyph->getQuadAtlasBounds(atlasLeft, atlasBottom, atlasRight, atlasTop);
      glm::vec2 texCoordMin((float)atlasLeft, (float)atlasBottom);
      glm::vec2 texCoordMax((float)atlasRight, (float)atlasTop);

      double planeLeft, planeBottom, planeRight, planeTop;
      glyph->getQuadPlaneBounds(planeLeft, planeBottom, planeRight, planeTop);
      glm::vec2 quadMin((float)planeLeft, (float)planeBottom);
      glm::vec2 quadMax((float)planeRight, (float)planeTop);

      // scale
      quadMin *= fsScale, quadMax *= fsScale;
      // offset
      quadMin += glm::vec2(x, y);
      quadMax += glm::vec2(x, y);
      float texelWidth = 1.0f / m_fontAtlasTexture->getWidth();
      float texelHeight = 1.0f / m_fontAtlasTexture->getHeight();
      texCoordMin *= glm::vec2(texelWidth, texelHeight);
      texCoordMax *= glm::vec2(texelWidth, texelHeight);

      allocateCharacter(glm::translate(glm::mat4(1.f), {position, 0.f}), color,
                        // textureCoordinate
                        {texCoordMin, glm::vec2(texCoordMin.x, texCoordMax.y),
                         texCoordMax, glm::vec2(texCoordMax.x, texCoordMin.y)},
                        // vertex positions
                        {glm::vec4{quadMin, 0.f, 1.f},
                         glm::vec4{quadMin.x, quadMax.y, 0.f, 1.f},
                         glm::vec4{quadMax, 0.f, 1.f},
                         glm::vec4{quadMax.x, quadMin.y, 0.f, 1.f}});

      if (*t != '\0') {
        double advance = glyph->getAdvance();
        char nextCharacter = *(t + 1);
        fontGeometry.getAdvance(advance, *t, nextCharacter);
        float kerningOffset = 0.0f;
        x += fsScale * advance + kerningOffset;
      }
      break;
    }
  }
}

} // namespace pain
