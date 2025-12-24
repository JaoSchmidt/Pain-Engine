#pragma once

#include "Core.h"

#include "CoreRender/Camera.h"
#include "CoreRender/Renderer/BatchCircles.h"
#include "CoreRender/Renderer/BatchQuad.h"
#include "CoreRender/Renderer/BatchSpray.h"
#include "CoreRender/Renderer/BatchText.h"
#include "CoreRender/Renderer/BatchTri.h"
#include "CoreRender/Renderer/MiscDebugGrid.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Text/Font.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "ECS/Components/Particle.h"
#include "Misc/BasicOrthoCamera.h"

namespace pain
{

class Scene;

struct Renderer2d {
  static Renderer2d createRenderer2d();
  Renderer2d &operator=(Renderer2d &&o) noexcept;
  // void changeCamera(const OrthographicMatrices &cameraMatrices);
  void changeCamera(const OrthographicMatrices &cameraMatrices,
                    reg::Entity camera);
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

    // replaced by m_textBatch.fontAtlas
    // const Texture *m_fontAtlasTexture = nullptr;
    const OrthographicMatrices *cameraMatrices = nullptr;
    reg::Entity orthoCameraEntity = reg::Entity{-1};
  };

  M m;

  Renderer2d(M args) : m(std::move(args)) {}
};

} // namespace pain
