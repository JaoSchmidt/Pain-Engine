#pragma once

#include "Core.h"

#include "CoreRender/CameraComponent.h"
#include "CoreRender/Renderer/BatchCircles.h"
#include "CoreRender/Renderer/BatchQuad.h"
#include "CoreRender/Renderer/BatchSpray.h"
#include "CoreRender/Renderer/BatchText.h"
#include "CoreRender/Renderer/BatchTri.h"
#include "CoreRender/Renderer/MiscDebugGrid.h"
#include "CoreRender/Text/Font.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Components/Particle.h"
#include "ECS/Registry/Entity.h"

namespace pain
{

// Frwd declare Scene
template <reg::CompileTimeBitMaskType Manager> class AbstractScene;
using Scene = AbstractScene<ComponentManager>;
template <reg::CompileTimeBitMaskType Manager> class AbstractScene;
using UIScene = AbstractScene<UIManager>;

struct Renderer2d {
  struct Stats {
    const uint32_t &count;
    uint32_t indices;
    uint32_t vertices;
    const uint32_t &draws;
    const char *name;
  };
  static Renderer2d createRenderer2d();
  Renderer2d &operator=(Renderer2d &&o) noexcept;
  // void changeCamera(const OrthographicMatrices &cameraMatrices);
  void changeCamera(reg::Entity camera);
  // ================================================================= //
  // Renderer basic wrapper around opengl
  // ================================================================= //

  void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);
  void beginScene(DeltaTime globalTime, const Scene &scene,
                  const glm::mat4 &transform = glm::mat4(1.0f));
  void endScene();
  void setViewport(int x, int y, int width, int height);
  void setClearColor(const glm::vec4 &color);
  void clear();
  void clearEntireRenderer();

  // ================================================================= //
  // Draw Circles
  // ================================================================= //

  /** Draws a circle */
  void drawCircle(const glm::vec2 &position, const float radius,
                  const glm::vec4 &tintColor,
                  const std::array<glm::vec2, 4> &textureCoordinate = {
                      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                      glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
  // ================================================================= //
  // Draw Quads
  // ================================================================= //

  /** Draws a quad */
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const glm::vec4 &tintColor, Texture &texture,
                const float tilingFactor = 1.0f,
                const std::array<glm::vec2, 4> &textureCoordinate = {
                    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                    glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

  /** Draws a quad with rotation, new rotationRadians (angle)*/
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const glm::vec4 &tintColor, const float rotationRadians,
                Texture &texture, const float tilingFactor = 1.0f,
                const std::array<glm::vec2, 4> &textureCoordinate = {
                    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                    glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});
  // ================================================================= //
  // Draw Tri
  // ================================================================= //

  void drawTri(const glm::vec2 &position, const glm::vec2 &size,
               const glm::vec4 &tintColor);

  /** Draws triangles with rotation, new rotationRadians (angle)*/
  void drawTri(const glm::vec2 &position, const glm::vec2 &size,
               const glm::vec4 &tintColor, const float rotationRadians);

  /** Draws a bunch of particles in a spray format */
  void beginSprayParticle(const DeltaTime globalTime,
                          const ParticleSprayComponent &particleSprayComponent);
  void drawSprayParticle(const Particle &p);

  /** Draws a string of glyphs from a font atlas */
  void drawString(const glm::vec2 &position, const char *string,
                  const Font &font, const glm::vec4 &color);

  const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size,
                               const float rotationRadians);
  const glm::mat4 getTransform(const glm::vec2 &position,
                               const glm::vec2 &size);

  void removeTexture(const Texture &texture);
  void setCellGridSize(float size);

  // // TODO:(jao) search MaxTextureSlots dinamically (i.e TMU value on gpu)
  static constexpr uint32_t MaxTextureSlots = 32;

  template <typename Batch>
    requires requires(Batch &b) { b.statsCount; }
  Stats getStatistics()
  {
    if constexpr (std::is_same_v<Batch, TriBatch>)
      return {m.triBatch.statsCount, m.triBatch.statsCount * 3,
              m.triBatch.statsCount * 3, m.triBatch.drawCount, "Triangules"};
    else if constexpr (std::is_same_v<Batch, QuadBatch>)
      return {m.quadBatch.statsCount, m.quadBatch.statsCount * 6,
              m.quadBatch.statsCount * 4, m.quadBatch.drawCount, "Quads"};
    else if constexpr (std::is_same_v<Batch, TextBatch>)
      return {m.textBatch.statsCount, m.textBatch.statsCount * 6,
              m.textBatch.statsCount * 4, m.textBatch.drawCount, "Glyphs"};
    else if constexpr (std::is_same_v<Batch, CircleBatch>)
      return {m.circleBatch.statsCount, m.circleBatch.statsCount * 6,
              m.circleBatch.statsCount * 4, m.circleBatch.drawCount, "Circles"};
    else if constexpr (std::is_same_v<Batch, SprayBatch>)
      return {m.sprayBatch.statsCount, m.sprayBatch.statsCount * 6,
              m.sprayBatch.statsCount * 4, m.sprayBatch.drawCount, "Sprays"};
  }

private:
  void flush();
  void uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                           DeltaTime globalTime, const glm::mat4 &transform,
                           const glm::ivec2 &resolution,
                           const glm::vec2 &cameraPos, const float zoomLevel);
  void bindTextures();
  void goBackToFirstVertex();
  float allocateTextures(Texture &texture);

  struct M {
    QuadBatch quadBatch;
    TriBatch triBatch;
    CircleBatch circleBatch;
    SprayBatch sprayBatch;
    TextBatch textBatch;
    DebugGrid debugGrid;
    // texture initializer
    Texture *whiteTexture = nullptr;
    std::array<Texture *, MaxTextureSlots> textureSlots;
    uint32_t textureSlotIndex = 1; // at init, there is 1 white texture

    reg::Entity orthoCameraEntity = reg::Entity{-1};
    // replaced by m_textBatch.fontAtlas
    // const Texture *m_fontAtlasTexture = nullptr;
  };

  M m;

  Renderer2d(M args) : m(std::move(args)) {}
};

} // namespace pain
