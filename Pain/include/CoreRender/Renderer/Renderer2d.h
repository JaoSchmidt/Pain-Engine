/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Assets/ManagerMaterial.h"
#include "Core.h"

#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Buffers/VertexArray.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/Renderer/BatchQuad.h"
#include "CoreRender/Renderer/BatchSprayParticles.h"
#include "CoreRender/Renderer/BatchText.h"
#include "CoreRender/Renderer/BatchTri.h"
#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Renderer/MiscDebugGrid.h"
#include "CoreRender/Renderer/Stats.h"
#include "CoreRender/Text/Font.h"
#include "ECS/Registry/Entity.h"
#include "Physics/Particles/SprayCmp.h"
#include "TextComponent.h"
#include <array>
#include <string_view>

namespace pain
{

// Frwd declare Scene
class Scene;
class UIScene;
struct Transform2dComponent;

/**
 * @brief 2D renderer facade built on top of batched OpenGL rendering.
 *
 * Renderer2d owns and coordinates multiple batch renderAPI (quads, circles,
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
  ~Renderer2d();
  NONCOPYABLE(Renderer2d);
  NONMOVABLE(Renderer2d);
  /// @brief Factory function to create a renderer instance.
  static Renderer2d createRenderer2d(MaterialManager &materialManager);

  // Renderer2d(Renderer2d &&o) noexcept;
  // Renderer2d &operator=(Renderer2d &&o) noexcept;
  /// @brief Change the active camera entity used for rendering.
  void changeCamera(reg::Entity camera);
  /// @brief Returns true if a valid camera is currently bound.
  bool hasCamera() const;

  // ================================================================= //
  // Renderer basic wrapper around OpenGL
  // ================================================================= //

  /// @brief Immediately draw and end the current scene using a vertex array.
  void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);

  /**
   * @brief Begin a new rendering scene.
   *
   * @param globalTime Global engine time.
   * @param perspCamera Perspecitve camera component
   * @param position camera position
   */
  void beginScene(DeltaTime globalTime, const cmp::OrthoCamera &cc,
                  const Transform2dComponent &tc);

  // @brief Flush all batches and finalize the scene.
  void endScene(DeltaTime globalTime, const cmp::OrthoCamera &cc,
                const Transform2dComponent &tc);

  /// @brief Clears all renderer state and internal caches.
  void clearEntireRenderer();

  // ================================================================= //
  // Submit Quads
  // ================================================================= //

  /// @brief Submit an axis-aligned textured quad.
  void submitQuad(const glm::vec2 &position, float size, RenderLayer layer,
                  const Material &material);

  /// @brief Submit an axis-aligned textured quad. Override color
  void submitQuad(const glm::vec2 &position, float size, RenderLayer layer,
                  const Material &material, Color overrideColor);

  /**
   * @brief Submit a rotated textured quad. Quads, compared to rect are
   * instanced. Meaning they are faster
   *
   * @param rotationRadians Rotation angle in radians.
   */
  void submitQuad(const glm::vec2 &position, float size, float rotationRadians,
                  RenderLayer layer, const Material &material);

  void submitQuad(const glm::vec2 &position, float size, float rotationRadians,
                  RenderLayer layer, const Material &material,
                  Color overrideColor);

  /// @brief Submit an axis-aligned textured quad.
  void submitQuad(const glm::mat4 &transform, RenderLayer layer,
                  const Material &material);

  /// @brief Submit an axis-aligned textured quad. Override color
  void submitQuad(const glm::mat4 &transform, RenderLayer layer,
                  const Material &material, Color overrideColor);

  // ================================================================= //
  // Submit Rect
  // ================================================================= //

  /// @brief Submit an axis-aligned textured rect.
  void submitRect(const glm::vec2 &position, const glm::vec2 &size,
                  RenderLayer layer, const Material &material);

  /// @brief Submit an axis-aligned textured rect. Override color
  void submitRect(const glm::vec2 &position, const glm::vec2 &size,
                  RenderLayer layer, const Material &material,
                  Color overrideColor);

  /**
   * @brief Submit a rotated textured rect. With color override
   *
   * @param rotationRadians Rotation angle in radians.
   */
  void submitRect(const glm::vec2 &position, const glm::vec2 &size,
                  float rotationRadians, RenderLayer layer,
                  const Material &material);

  void submitRect(const glm::vec2 &position, const glm::vec2 &size,
                  float rotationRadians, RenderLayer layer,
                  const Material &material, Color overrideColor);

  /// @brief Submit an axis-aligned textured rect.
  void submitRect(const glm::mat4 &transform, RenderLayer layer,
                  const Material &material);

  /// @brief Submit an axis-aligned textured rect. Override color
  void submitRect(const glm::mat4 &transform, RenderLayer layer,
                  const Material &material, Color overrideColor);

  // ================================================================= //

  // ================================================================= //
  // Submit Line
  // ================================================================= //

  /// @brief Submit a line with specific material
  void submitLine(const glm::vec2 &origin, const glm::vec2 &destination,
                  float thickness, RenderLayer layer, const Material &material);

  /// @brief Submit a line with specific material. Override color
  void submitLine(const glm::vec2 &origin, const glm::vec2 &destination,
                  float thickness, RenderLayer layer, const Material &material,
                  Color overrideColor);

  // ================================================================= //
  // Submit Triangles
  // ================================================================= //

  /// @brief Submit a colored triangle primitive.
  void submitTri(const glm::vec2 &position, const glm::vec2 &size,
                 RenderLayer layer, const Material &material);

  /// @brief Submit a colored triangle primitive. Override color
  void submitTri(const glm::vec2 &position, const glm::vec2 &size,
                 RenderLayer layer, const Material &material,
                 Color overrideColor);

  /// @brief Submit a rotated triangle primitive.
  void submitTri(const glm::vec2 &position, const glm::vec2 &size,
                 float rotationRadians, RenderLayer layer,
                 const Material &material);

  /// @brief Submit a rotated triangle primitive. Override color
  void submitTri(const glm::vec2 &position, const glm::vec2 &size,
                 float rotationRadians, RenderLayer layer,
                 const Material &material, Color overrideColor);

  void submitTri(const glm::mat4 &transform, RenderLayer layer,
                 const Material &material);

  /// @brief Submit a triangle from transform. Override color
  void submitTri(const glm::mat4 &transform, RenderLayer layer,
                 const Material &material, Color overrideColor);
  // ================================================================= //
  // Particles
  // ================================================================= //

  /// @brief Begin rendering a particle spray batch.
  void beginSprayParticle(const ParticleSprayComponent &psc);

  /// @brief Submit a single particle to the current spray batch.
  void submitSprayParticle(const SprayParticle &p);

  // ================================================================= //
  // Text
  // ================================================================= //

  /// @brief Draw a UTF-8 string using a font atlas.
  void submitString(const glm::vec2 &position, float scale,
                    const std::string_view &text, const Font &font, Color color,
                    TextAlign = TextAlign::Left);
  void submitString(const glm::mat4 &transform, const std::string_view &text,
                    const Font &font, Color color, TextAlign = TextAlign::Left);

  // ================================================================= //
  // Transforms
  // ================================================================= //

  /// @brief Build a transform matrix with rotation.
  static glm::mat4 getTransform(const glm::vec2 &position,
                                const glm::vec2 &size, float rotationRadians);

  /// @brief Build a transform matrix without rotation.
  static glm::mat4 getTransform(const glm::vec2 &position,
                                const glm::vec2 &size);

  /// @brief Build a transform matrix with rotation.
  static glm::mat4 getUniformTransform(const glm::vec2 &position, float scale,
                                       float rotationRadians);

  /// @brief Build a transform matrix without rotation.
  static glm::mat4 getUniformTransform(const glm::vec2 &position, float scale);

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

  /// @brief Retrieve rendering statistics for a specific batch type.
  Stats getQuadStatistics();
  /// @brief Retrieve rendering statistics for a specific batch type.
  Stats getTriStatistics();
  /// @brief Retrieve rendering statistics for a specific batch type.
  Stats getSprayStatistics();
  /// @brief Retrieve rendering statistics for a specific batch type.
  Stats getTextStatistics();

private:
  template <typename Batch>
    requires requires(Batch &b) { b.statsCount; }
  Stats getStatistics(Batch &b)
  {
    if constexpr (std::is_same_v<Batch, TriBatch>)
      return {"Triangles", b.statsCount, b.statsCount * 3, b.statsCount * 3,
              b.drawCount};
    else if constexpr (std::is_same_v<Batch, TextBatch>)
      return {"Glyphs", m.textBatch.statsCount, m.textBatch.statsCount * 6,
              m.textBatch.statsCount * 4, m.textBatch.drawCount};
    else if constexpr (std::is_same_v<Batch, SprayBatch>)
      return {"Sprays", m.sprayBatch.statsCount, m.sprayBatch.statsCount * 6,
              m.sprayBatch.statsCount * 4, m.sprayBatch.drawCount};
    else if constexpr (std::is_same_v<Batch, QuadBatch>) {
      return {"Quads", b.statsCount, b.statsCount * 6, b.statsCount * 4,
              b.drawCount};
    }
  }

  static float constexpr smallSpacingOrder(short order)
  {
    return static_cast<float>(order) / 1024.F;
  };
  void flush();
  void uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                           DeltaTime globalTime, const glm::ivec2 &resolution,
                           const glm::vec2 &cameraPos, float zoomLevel);
  void bindTextures();
  float allocateTextures(Texture &texture);

  struct M {
    std::reference_wrapper<MaterialManager> materialManager;
    SprayBatch sprayBatch;
    TextBatch textBatch;
    DebugGrid debugGrid;

    // texture initializer
    Texture *whiteTexture = nullptr;
    Texture **textureSlots;
    uint32_t textureSlotIndex = 1; // at init, there is 1 white texture
    reg::Entity orthoCameraEntity = reg::Entity{-1};
    // replaced by m_textBatch.fontAtlas
  };

  M m;
  template <typename NRVO>
    requires std::same_as<std::invoke_result_t<NRVO>, M>
  Renderer2d(NRVO &&factory) : m(factory()){};
  friend class Application;
};

} // namespace pain
