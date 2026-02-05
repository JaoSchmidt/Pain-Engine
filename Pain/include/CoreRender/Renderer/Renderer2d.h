/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"

#include "CoreRender/Renderer/BatchCircles.h"
#include "CoreRender/Renderer/BatchQuad.h"
#include "CoreRender/Renderer/BatchSpray.h"
#include "CoreRender/Renderer/BatchText.h"
#include "CoreRender/Renderer/BatchTri.h"
#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Renderer/MiscDebugGrid.h"
#include "CoreRender/Text/Font.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "ECS/Registry/Entity.h"
#include "Physics/Particles/SprayCmp.h"

namespace pain
{

// Frwd declare Scene
class Scene;
class UIScene;

/**
 * @brief 2D renderer facade built on top of batched OpenGL rendering.
 *
 * Renderer2d owns and coordinates multiple batch renderers (quads, circles,
 * triangles, text, particles, debug grid) and provides a simple API for
 * drawing primitives inside a scene.
 *
 * Typical usage:
 *  - Create using createRenderer2d().
 *  - Call beginScene().
 *  - Issue draw calls.
 *  - Call endScene().
 */
struct Renderer2d {
  /// @brief Aggregated rendering statistics for a batch type.
  struct Stats {
    uint32_t count;
    uint32_t indices;
    uint32_t vertices;
    uint32_t draws;
    const char *name;
  };
  /// @brief Factory function to create a renderer instance.
  static Renderer2d createRenderer2d();
  Renderer2d &operator=(Renderer2d &&o) noexcept;
  /// @brief Change the active camera entity used for rendering.
  void changeCamera(reg::Entity camera);
  /// @brief Returns true if a valid camera is currently bound.
  bool hasCamera();

  // ================================================================= //
  // Renderer basic wrapper around OpenGL
  // ================================================================= //

  /// @brief Immediately draw and end the current scene using a vertex array.
  void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);

  /**
   * @brief Begin a new rendering scene.
   *
   * @param globalTime Global engine time.
   * @param scene      Scene being rendered.
   * @param transform  Optional root transform applied to all draws.
   */
  void beginScene(DeltaTime globalTime, const Scene &scene,
                  const glm::mat4 &transform = glm::mat4(1.0f));

  // @brief Flush all batches and finalize the scene.
  void endScene();

  void setViewport(int x, int y, int width, int height);
  void setClearColor(const glm::vec4 &color);
  void clear();

  /// @brief Clears all renderer state and internal caches.
  void clearEntireRenderer();

  // ================================================================= //
  // Draw Circles
  // ================================================================= //

  /**
   * @brief Draw a 2D circle.
   *
   * @param position Circle center in world space.
   * @param radius   Circle radius.
   * @param tintColor Color multiplier.
   * @param textureCoordinate Optional texture coordinates.
   */
  void drawCircle(const glm::vec2 &position, const float radius,
                  const Color &tintColor,
                  const std::array<glm::vec2, 4> &textureCoordinate = {
                      glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                      glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

  // ================================================================= //
  // Draw Quads
  // ================================================================= //

  /// @brief Draw an axis-aligned textured quad.
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const Color &tintColor, RenderLayer layer, Texture &texture,
                const float tilingFactor = 1.0f,
                const std::array<glm::vec2, 4> &textureCoordinate = {
                    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                    glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

  /**
   * @brief Draw a rotated textured quad.
   *
   * @param rotationRadians Rotation angle in radians.
   */
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const Color &tintColor, const float rotationRadians,
                RenderLayer layer, Texture &texture,
                const float tilingFactor = 1.0f,
                const std::array<glm::vec2, 4> &textureCoordinate = {
                    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                    glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)});

  // ================================================================= //
  // Draw Triangles
  // ================================================================= //

  /// @brief Draw a colored triangle primitive.
  void drawTri(const glm::vec2 &position, const glm::vec2 &size,
               const glm::vec4 &tintColor);

  /// @brief Draw a rotated triangle primitive.
  void drawTri(const glm::vec2 &position, const glm::vec2 &size,
               const glm::vec4 &tintColor, const float rotationRadians);

  // ================================================================= //
  // Particles
  // ================================================================= //

  /// @brief Begin rendering a particle spray batch.
  void beginSprayParticle(const ParticleSprayComponent &particleSprayComponent);

  /// @brief Submit a single particle to the current spray batch.
  void drawSprayParticle(const SprayParticle &p);

  // ================================================================= //
  // Text
  // ================================================================= //

  /// @brief Draw a UTF-8 string using a font atlas.
  void drawString(const glm::vec2 &position, const char *string,
                  const Font &font, const glm::vec4 &color);

  // ================================================================= //
  // Transforms
  // ================================================================= //

  /// @brief Build a transform matrix with rotation.
  const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size,
                               const float rotationRadians);

  /// @brief Build a transform matrix without rotation.
  const glm::mat4 getTransform(const glm::vec2 &position,
                               const glm::vec2 &size);

  // ================================================================= //
  // Resources / Debug
  // ================================================================= //

  /// @brief Remove a texture from the internal texture slot cache.
  void removeTexture(const Texture &texture);

  /**
   * @brief Enable or disable the debug grid.
   *
   * @param size Cell size. Pass 0 to disable the grid.
   */
  void setCellGridSize(float size);

  /// @brief Maximum number of texture slots supported per batch.
  static constexpr uint32_t MaxTextureSlots = 32;

  /**
   * @brief Retrieve rendering statistics for a specific batch type.
   *
   * Example:
   * @code
   * auto stats = renderer.getStatistics<QuadBatch>();
   * @endcode
   */
  template <typename Batch>
    requires requires(Batch &b) { b.statsCount; }
  Stats getStatistics()
  {
    if constexpr (std::is_same_v<Batch, TriBatch>)
      return {m.triBatch.statsCount, m.triBatch.statsCount * 3,
              m.triBatch.statsCount * 3, m.triBatch.drawCount, "Triangules"};
    else if constexpr (std::is_same_v<Batch, TextBatch>)
      return {m.textBatch.statsCount, m.textBatch.statsCount * 6,
              m.textBatch.statsCount * 4, m.textBatch.drawCount, "Glyphs"};
    else if constexpr (std::is_same_v<Batch, CircleBatch>)
      return {m.circleBatch.statsCount, m.circleBatch.statsCount * 6,
              m.circleBatch.statsCount * 4, m.circleBatch.drawCount, "Circles"};
    else if constexpr (std::is_same_v<Batch, SprayBatch>)
      return {m.sprayBatch.statsCount, m.sprayBatch.statsCount * 6,
              m.sprayBatch.statsCount * 4, m.sprayBatch.drawCount, "Sprays"};
    else if constexpr (std::is_same_v<Batch, QuadBatch>) {
      uint32_t statsCount = 0;
      uint32_t drawCount = 0;
      for (QuadBatch &batch : m.quadBatches) {
        statsCount += batch.statsCount;
        drawCount += batch.drawCount;
      }
      return {statsCount, statsCount * 6, statsCount * 4, drawCount, "Quads"};
    }
  }

private:
  float constexpr smallSpacingOrder(short order) { return order / 1024.f; };
  void flush();
  void uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                           DeltaTime globalTime, const glm::mat4 &transform,
                           const glm::ivec2 &resolution,
                           const glm::vec2 &cameraPos, const float zoomLevel);
  void bindTextures();
  void goBackToFirstVertex();
  float allocateTextures(Texture &texture);

  struct M {
    std::array<QuadBatch, NumLayers> quadBatches;
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
  template <typename NRVO>
    requires std::same_as<std::invoke_result_t<NRVO>, M>
  Renderer2d(NRVO &&factory) : m(factory()){};
  friend class Application;
};

} // namespace pain
