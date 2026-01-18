/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/Renderer2d.h"
#include "Assets/ManagerTexture.h"
#include "CoreRender/CameraComponent.h"
#include "Debugging/Profiling.h"

#include "ECS/WorldScene.h"
#include "Physics/MovementComponent.h"
#include "glm/ext/matrix_transform.hpp"

#include "platform/ContextBackend.h"
namespace pain
{
// TODO: use struct for draw parameters
// struct QuadDrawParams {
//   glm::vec2 position{0.f, 0.f};
//   glm::vec2 size{1.0f, 1.0f};
//   Color tintColor{255};
//   float tilingFactor = 1.0f;
//   RenderLayer layer = RenderLayer::Default;
//   Texture *texture;
//   std::array<glm::vec2, 4> texCoords = {
//       glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
//       glm::vec2(0.0f, 1.0f)};
// };
// struct CircleDrawParams {
//   glm::vec2 position{0.f, 0.f};
//   glm::vec2 size{0.1f, 0.1f};
//   glm::vec4 tintColor{1.0f};
//   std::array<glm::vec2, 4> texCoords = {
//       glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
//       glm::vec2(0.0f, 1.0f)};
// };

extern const Texture *m_fontAtlasTexture;

// ================================================================= //
// Renderer: basic wrapper around opengl
// ================================================================= //
// void Renderer2d::shutdown() {}

void Renderer2d::setViewport(int x, int y, int width, int height)
{
  backend::setViewPort(x, y, width, height);
}

void Renderer2d::clear() { backend::clear(); }

void Renderer2d::setClearColor(const glm::vec4 &color)
{
  backend::setClearColor(color);
}
bool Renderer2d::hasCamera() { return m.orthoCameraEntity != reg::Entity{-1}; }
void Renderer2d::changeCamera(reg::Entity cameraEntity)
{
  m.orthoCameraEntity = cameraEntity;
}

void Renderer2d::beginScene(DeltaTime globalTime, const Scene &scene,
                            const glm::mat4 &transform)
{
  PROFILE_FUNCTION();
  const cmp::OrthoCamera &cc =
      std::as_const(scene).getComponent<Component::OrthoCamera>(
          m.orthoCameraEntity);
  const Transform2dComponent &tc =
      std::as_const(scene).getComponent<Transform2dComponent>(
          m.orthoCameraEntity);
  uploadBasicUniforms(cc.getViewProjectionMatrix(), globalTime, transform,
                      cc.getResolution(), tc.m_position, cc.m_zoomLevel);
  goBackToFirstVertex();
}

void Renderer2d::flush()
{
  PROFILE_FUNCTION();
  // bindTextures();
  for (uint8_t i = 0; i < NumLayers; i++) {
    m.quadBatches[i].flush(m.textureSlots, m.textureSlotIndex);
  }
  m.circleBatch.flush();
  m.sprayBatch.flush();
  m.triBatch.flush();
  m.textBatch.flush();

  m.debugGrid.flush();
}

void Renderer2d::endScene()
{
  // quadBatch->sendAllDataToOpenGL();
  // NOTE: sendAllDataToOpenGL probably won't be here in the future,
  // otherwise flush() wouldn't need to be a function
  flush();
}

// ================================================================= //
// Draw Circles
// ================================================================= //

void Renderer2d::drawCircle(const glm::vec2 &position, const float diameter,
                            const Color &tintColor,
                            const std::array<glm::vec2, 4> &textureCoordinate)
{
  PROFILE_FUNCTION();
  if (m.circleBatch.indexCount >= QuadBatch::MaxIndices) {
    m.circleBatch.flush();
    m.circleBatch.resetPtr();
  }
  const glm::mat4 transform =
      getTransform(position, glm::vec2(diameter, diameter));
  m.circleBatch.allocateCircle(transform, tintColor, textureCoordinate);
}

// ================================================================= //
// Draw Quads
// ================================================================= //

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const Color &tintColor, RenderLayer layer,
                          Texture &texture, float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  PROFILE_FUNCTION();
  QuadBatch &batch = m.quadBatches[static_cast<uint8_t>(layer)];

  if (batch.indexCount >= QuadBatch::MaxIndices) {
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size);
  batch.allocateQuad(transform, tintColor, tilingFactor, texIndex,
                     textureCoordinate);
}

void Renderer2d::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                          const Color &tintColor, const float rotationRadians,
                          RenderLayer layer, Texture &texture,
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  PROFILE_FUNCTION();
  QuadBatch &batch = m.quadBatches[static_cast<uint8_t>(layer)];

  if (batch.indexCount >= QuadBatch::MaxIndices) {
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size, rotationRadians);
  batch.allocateQuad(transform, tintColor, tilingFactor, texIndex,
                     textureCoordinate);
}

// ================================================================= //
// Draw Tri
// ================================================================= //

void Renderer2d::drawTri(const glm::vec2 &position, const glm::vec2 &size,
                         const glm::vec4 &tintColor)
{
  if (m.triBatch.indexCount >= TriBatch::MaxIndices) {
    m.triBatch.flush();
    m.triBatch.resetPtr();
  }
  PROFILE_FUNCTION();
  const glm::mat4 transform = getTransform(position, size);
  m.triBatch.allocateTri(transform, tintColor);
}
void Renderer2d::drawTri(const glm::vec2 &position, const glm::vec2 &size,
                         const glm::vec4 &tintColor,
                         const float rotationRadians)
{
  if (m.triBatch.indexCount >= TriBatch::MaxIndices) {
    m.triBatch.flush();
    m.triBatch.resetPtr();
  }
  PROFILE_FUNCTION();
  const glm::mat4 transform = getTransform(position, size, rotationRadians);
  m.triBatch.allocateTri(transform, tintColor);
}

// ================================================================= //
// Draw Spray Particles
// ================================================================= //

void Renderer2d::drawSprayParticle(const Particle &p)
{
  if (m.sprayBatch.indexCount >= SprayBatch::MaxIndices) {
    m.sprayBatch.flush();
    m.sprayBatch.resetPtr();
  }
  m.sprayBatch.allocateSprayParticles(p.m_position, p.m_offset, p.m_normal,
                                      p.m_startTime, p.m_rotationSpeed);
}

// ================================================================= //
// Draw Text
// ================================================================= //
void Renderer2d::drawString(const glm::vec2 &position, const char *string,
                            const Font &font, const glm::vec4 &color)
{
  PROFILE_FUNCTION();
  const auto &fontGeometry = font.getFontGeometry();
  const auto &metrics = fontGeometry.getMetrics();
  const double &spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

  if (m.textBatch.indexCount >= TextBatch::MaxIndices) {
    m.textBatch.flush();
    m.textBatch.resetPtr();
  }

  m.textBatch.fontAtlas = &font.getAtlasTexture();

  double x = 0.0;
  double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
  double y = 0.0;
  float lineHeightOffset = 0.0f;
  for (const char *letter = string; *letter != '\0'; letter++) {
    switch (*letter) {
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
      auto glyph = fontGeometry.getGlyph(static_cast<unsigned>(*letter));
      if (!glyph) {
        PLOG_E("Glyph '{}' not available on font family", *letter);
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
      float texelWidth =
          1.0f / static_cast<float>(m.textBatch.fontAtlas->getWidth());
      float texelHeight =
          1.0f / static_cast<float>(m.textBatch.fontAtlas->getHeight());
      texCoordMin *= glm::vec2(texelWidth, texelHeight);
      texCoordMax *= glm::vec2(texelWidth, texelHeight);

      m.textBatch.allocateCharacter(
          glm::translate(glm::mat4(1.f), {position, 0.f}), color,
          // textureCoordinate
          {texCoordMin, glm::vec2(texCoordMin.x, texCoordMax.y), texCoordMax,
           glm::vec2(texCoordMax.x, texCoordMin.y)},
          // vertex positions
          {glm::vec4{quadMin, 0.f, 1.f},
           glm::vec4{quadMin.x, quadMax.y, 0.f, 1.f},
           glm::vec4{quadMax, 0.f, 1.f},
           glm::vec4{quadMax.x, quadMin.y, 0.f, 1.f}});

      if (*letter != '\0') {
        double advance = glyph->getAdvance();
        unsigned nextCharacter = static_cast<unsigned>(*(letter + 1));
        fontGeometry.getAdvance(advance, static_cast<unsigned>(*letter),
                                nextCharacter);
        float kerningOffset = 0.0f;
        x += fsScale * advance + kerningOffset;
      }
      break;
    }
  }
}

// ================================================================= //
// Old draw2d.cpp functions
// ================================================================= //
Renderer2d Renderer2d::createRenderer2d()
{
  PROFILE_FUNCTION();

  backend::InitRenderer();

  return Renderer2d([] {
    return M{
        .quadBatches =
            {
                QuadBatch::create(),
                QuadBatch::create(),
                QuadBatch::create(),
                QuadBatch::create(),
                QuadBatch::create(),
                QuadBatch::create(),
                QuadBatch::create(),
            },
        .triBatch = TriBatch::create(),       //
        .circleBatch = CircleBatch::create(), //
        .sprayBatch = SprayBatch::create(),   //
        .textBatch = TextBatch::create(),     //
        .debugGrid = DebugGrid::create(),
        .textureSlots = // First texture is a  1x1 white texture
        {&TextureManager::getDefaultTexture(
            TextureManager::DefaultTexture::Blank)} //
    };
  }); //
}

void Renderer2d::bindTextures()
{
  PROFILE_FUNCTION();
  for (uint32_t i = 0; i < m.textureSlotIndex; i++)
    m.textureSlots[i]->bindToSlot(i);
}

void Renderer2d::goBackToFirstVertex()
{
  PROFILE_FUNCTION();
  for (uint8_t i = 0; i < NumLayers; i++) {
    m.quadBatches[i].resetAll();
  }
  m.textBatch.resetAll();
  m.sprayBatch.resetAll();
  m.circleBatch.resetAll();
  m.triBatch.resetAll();
}

void Renderer2d::uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                                     DeltaTime globalTime,
                                     const glm::mat4 &transform,
                                     const glm::ivec2 &resolution,
                                     const glm::vec2 &cameraPos,
                                     float zoomLevel)
{
  PROFILE_FUNCTION();
  for (uint8_t i = 0; i < NumLayers; i++) {
    m.quadBatches[i].shader.bind();
    m.quadBatches[i].shader.uploadUniformMat4("u_ViewProjection",
                                              viewProjectionMatrix);
    m.quadBatches[i].shader.uploadUniformMat4("u_Transform", transform);
  }

  m.triBatch.shader.bind();
  m.triBatch.shader.uploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
  m.triBatch.shader.uploadUniformMat4("u_Transform", transform);

  m.sprayBatch.shader.bind();
  m.sprayBatch.shader.uploadUniformMat4("u_ViewProjection",
                                        viewProjectionMatrix);
  m.sprayBatch.shader.uploadUniformMat4("u_Transform", transform);
  m.sprayBatch.shader.uploadUniformFloat("u_Time", globalTime.getSecondsf());

  m.textBatch.shader.bind();
  m.textBatch.shader.uploadUniformMat4("u_ViewProjection",
                                       viewProjectionMatrix);
  m.textBatch.shader.uploadUniformMat4("u_Transform", transform);

  m.debugGrid.shader.bind();
  m.debugGrid.shader.uploadUniformFloat("u_zoomLevel", zoomLevel);
  m.debugGrid.shader.uploadUniformFloat2("u_cameraPos", cameraPos);
  m.debugGrid.shader.uploadUniformFloat("u_resolution_y", (float)resolution.y);
  m.debugGrid.shader.uploadUniformMat4("u_ViewProjection",
                                       viewProjectionMatrix);
  // m_debugGrid.shader.uploadUniformMat4("u_Transform", transform);
  // m_debugGrid.shader.uploadUniformFloat2("u_resolution",
  // glm::vec2(resolution));

  // float cellScreenPixels = (1.f / zoomLevel) * resolution.y;
  // PLOG_I("cellScreenPixels inside BasicUniform= {}", cellScreenPixels);
  m.circleBatch.shader.bind();
  m.circleBatch.shader.uploadUniformMat4("u_ViewProjection",
                                         viewProjectionMatrix);
  m.circleBatch.shader.uploadUniformMat4("u_Transform", transform);
}

float Renderer2d::allocateTextures(Texture &texture)
{
  PROFILE_FUNCTION();
  float textureIndex = 0.0f;
  // use it to allocate new texture
  if (texture.m_slot == 0) {
    textureIndex = (float)m.textureSlotIndex;
    m.textureSlots[m.textureSlotIndex] = &texture;
    texture.m_slot = m.textureSlotIndex;
    m.textureSlotIndex++;
  } else {
    textureIndex = (float)texture.m_slot;
  }
  // TODO: check if m_textureSlotIndex is bigger than 32, then flush

  P_ASSERT_W(textureIndex != 0.0f,
             "Missing texture inside a drawQuad that requires textures");
  return textureIndex;
}
void Renderer2d::removeTexture(const Texture &texture)
{
  PROFILE_FUNCTION();
  if (texture.m_slot == 0) // m_textureSlots doesn't have the texture
    return;
  P_ASSERT_W(*m.textureSlots[texture.m_slot] == texture,
             "Attempted to remove a texture that wasn't allocated.");
  for (uint32_t i = texture.m_slot; i < m.textureSlotIndex - 1; i++) {
    m.textureSlots[i] = m.textureSlots[i + 1];
    m.textureSlots[i]->m_slot = i; // Update the slot value in the Texture
  }

  m.textureSlots[m.textureSlotIndex] = nullptr; // Clear last slot
  m.textureSlotIndex--;
  return;
}
const glm::mat4 Renderer2d::getTransform(const glm::vec2 &position,
                                         const glm::vec2 &size)
{
  PROFILE_FUNCTION();
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}
const glm::mat4 Renderer2d::getTransform(const glm::vec2 &position,
                                         const glm::vec2 &size,
                                         const float rotationAngleRadians)
{
  PROFILE_FUNCTION();
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::rotate(glm::mat4(1.0f), rotationAngleRadians,
                     {0.0f, 0.0f, 1.0f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}

void Renderer2d::setCellGridSize(float cellsize)
{
  if (cellsize != 0) {
    m.debugGrid.enableGrid = true;
    m.debugGrid.shader.bind();
    m.debugGrid.shader.uploadUniformFloat("u_CellSize", 2 * cellsize);
  } else {
    m.debugGrid.enableGrid = false;
  }
}

void Renderer2d::beginSprayParticle(const DeltaTime globalTime,
                                    const ParticleSprayComponent &psc)
{
  PROFILE_FUNCTION();
  m.sprayBatch.shader.bind();
  m.sprayBatch.shader.uploadUniformFloat("u_Time", globalTime.getSecondsf());
  m.sprayBatch.shader.uploadUniformFloat("u_ParticleVelocity", psc.m_velocity);
  m.sprayBatch.shader.uploadUniformFloat("u_LifeTime", (float)psc.m_lifeTime);
  m.sprayBatch.shader.uploadUniformFloat("u_SizeChangeSpeed",
                                         psc.m_sizeChangeSpeed);
  m.sprayBatch.shader.uploadUniformFloat("u_randomSizeFactor",
                                         psc.m_randSizeFactor);
}

} // namespace pain
