/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "CoreRender/Renderer/BatchCube.h"
#include "Physics/Particles/SprayCmp.h"
#include "pch.h"

#include "Core.h"

#include "CoreRender/VertexArray.h"
#include "Misc/BasicPerspCamera.h"

namespace pain
{

class Scene;
class UIScene;

class Renderer3d
{
public:
  /** @brief Aggregated rendering statistics for a batch type.*/
  /* Fields:
   * - count: number of objects
   * - indices: total index count
   * - vertices: total vertex count
   * - draws: draw calls issued
   * - name: batch name
   */
  struct Stats {
    uint32_t count;    ///< Number of objects in the batch
    uint32_t indices;  ///< Total index count submitted
    uint32_t vertices; ///< Total vertex count submitted
    uint32_t draws;    ///< Number of draw calls issued
    const char *name;  ///< Human-readable batch name
  };

  /// @brief Factory function to create a renderer instance.
  static Renderer3d createRenderer3d();
  Renderer3d &operator=(Renderer3d &&o) noexcept;
  /// @brief Change the active camera entity used for rendering.
  void changeCamera(reg::Entity camera);
  /// @brief Returns true if a valid camera is currently bound.
  bool hasCamera();

  // ================================================================= //
  // Renderer basic wrapper around OpenGL
  // ================================================================= //

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

  // ================================================================= //
  // Draw Triangles
  // ================================================================= //

  /// @brief Draw a colored triangle primitive.
  void drawCube(const glm::vec3 &position, const glm::vec3 &size,
                const Color &tintColor, Texture &texture, float tilingFactor,
                const std::array<glm::vec2, 4> &textureCoordinate);

  /// @brief Draw a rotated triangle primitive.
  void drawCube(const glm::vec3 &position, const glm::vec3 &size,
                const Color &tintColor, const glm::vec3 rotation,
                Texture &texture, float tilingFactor,
                const std::array<glm::vec2, 4> &textureCoordinate);

  // ================================================================= //
  // Particles
  // ================================================================= //

  // /// @brief Begin rendering a particle spray batch.
  // void beginSprayParticle(const ParticleSprayComponent
  // &particleSprayComponent);
  //
  // /// @brief Submit a single particle to the current spray batch.
  // void drawSprayParticle(const SprayParticle &p);

  // ================================================================= //
  // Transforms
  // ================================================================= //

  /// @brief Build a transform matrix without rotation.
  glm::mat4 getTransform(const glm::vec3 &position, const glm::vec3 &size);

  /// @brief Build a transform matrix with rotation.
  glm::mat4 getTransform(const glm::vec3 &position, const glm::vec3 &size,
                         const glm::vec3 &rotation);

  // ================================================================= //
  // Resources / Debug
  // ================================================================= //
  // TODO: Check if 2d and 3d texture interfere with each other when using
  // different TextureSlots members. Might need to use renderer context class
  // for handling textures

  /// @brief Remove a texture from the internal texture slot cache.
  void removeTexture(const Texture &texture);

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
    if constexpr (std::is_same_v<Batch, CubeBatch>) {
      return {m.cubeBatch.statsCount, m.cubeBatch.statsCount * 8,
              m.cubeBatch.statsCount * 3, m.cubeBatch.drawCount, "Triangules"};
    }
  }

private:
  float constexpr smallSpacingOrder(short order) { return order / 1024.f; };
  void flush();
  void uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                           DeltaTime globalTime, const glm::mat4 &transform,
                           const glm::ivec2 &resolution,
                           const glm::vec2 &cameraPos);
  void bindTextures();
  void goBackToFirstVertex();
  float allocateTextures(Texture &texture);

  struct M {
    CubeBatch cubeBatch;
    Texture *whiteTexture = nullptr;
    Texture **textureSlots;
    uint32_t textureSlotIndex = 1; // at init, there is 1 white texture

    reg::Entity orthoCameraEntity = reg::Entity{-1};
    // replaced by m_textBatch.fontAtlas
    // const Texture *m_fontAtlasTexture = nullptr;
  };

  M m;
  template <typename NRVO>
    requires std::same_as<std::invoke_result_t<NRVO>, M>
  Renderer3d(NRVO &&factory) : m(factory()){};
  friend class Application;
};

} // namespace pain
