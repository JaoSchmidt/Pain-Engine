/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "Assets/ManagerMaterial.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/Renderer/BatchCube.h"
#include "CoreRender/Renderer/BatchQuad.h"
#include "CoreRender/Renderer/BatchSphere.h"
#include "Physics/Particles/SprayCmp.h"
#include "pch.h"

#include "Core.h"

#include "CoreRender/Buffers/VertexArray.h"
#include "Misc/BasicPerspCamera.h"

namespace pain
{

class Scene;
class UIScene;
struct ResolvedMaterial;

/**
 * @brief 3D renderer facade built on top of batched OpenGL rendering.
 *
 * Renderer3d owns and coordinates multiple batch renderers like Cube and UV
 * Sphere and provides a simple API for drawing primitives inside a scene.
 *
 * Typical usage:
 *  - Create using createRenderer3d().
 *  - Call beginScene().
 *  - Issue submit calls.
 *  - Call endScene().
 */
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
    const char *name;      ///< Human-readable batch name
    uint32_t count = 0;    ///< Number of objects in the batch
    uint32_t indices = 0;  ///< Total index count submitted
    uint32_t vertices = 0; ///< Total vertex count submitted
    uint32_t draws = 0;    ///< Number of draw calls issued

    Stats &operator+=(const Stats &s)
    {
      count += s.count;
      indices += s.indices;
      vertices += s.vertices;
      draws += s.draws;
      return *this;
    }
    Stats operator+(const Stats &s) const
    {
      Stats result = *this;
      result += s;
      return result;
    }
  };

  /// @brief Factory function to create a renderer instance.
  static Renderer3d createRenderer3d(MaterialManager &materialManager);
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
  void endScene(const Scene &scene);

  // ================================================================= //
  // Draw Polygons
  // ================================================================= //

  /// @brief submit a cube polyhedron.
  // void submitCube(const glm::vec3 &position, float size,
  //                 const Material &material);

  // void submitCube(const glm::vec3 &position, float size, const Color &color,
  //                 Texture &texture, float tilingFactor);
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material);
  /// @brief submit a rotated cube polyhedron. Rotation dimensions are in
  /// radians
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material, const glm::vec3 &rotation);

  /// @brief submit a UV sphere polyhedron.
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material);
  /// @brief submit a rotating UV sphere polyhedron. Rotation dimensions are in
  /// radians
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material, const glm::vec3 &rotation);

  void submitLight(const glm::vec3 &pos, const Color &color);

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
  glm::mat4 getUniformScaleTransform(const glm::vec3 &position, float size);

  /// @brief Build a transform matrix with rotation.
  glm::mat4 getUniformScaleTransform(const glm::vec3 &position, float size,
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
  Stats getCubeStatistics();

private:
  float constexpr smallSpacingOrder(short order) { return order / 1024.f; };
  void flush();
  void uploadBasicUniforms(const Scene &scene,
                           const glm::mat4 &viewProjectionMatrix,
                           DeltaTime globalTime, const glm::mat4 &transform,
                           const glm::ivec2 &resolution,
                           const glm::vec3 &cameraPos);
  void bindTextures();
  // void beforeFlush(const MaterialKey &mat);
  float allocateTextures(Texture &texture);
  ResolvedMaterial resolveMaterial(const MaterialComponent &mat);

  template <typename Batch>
    requires requires(Batch &b) { b.statsCount; }
  Stats getStatistics(Batch &b)
  {
    if constexpr (std::is_same_v<Batch, CubeBatch>) {
      return {"Cubes", b.statsCount, b.statsCount * 8, b.statsCount * 3,
              b.drawCount};
    }
  }
  struct M {
    MaterialManager &materialManager;
    // std::array<SphereBatch, static_cast<uint8_t>(SphereDivision::Count)>
    //     sphereBatches;
    // CubeBatch cubeBatch;
    Texture *whiteTexture = nullptr;
    Texture **textureSlots;
    uint32_t textureSlotIndex = 1; // at init, there is 1 white texture

    reg::Entity lightEntity = reg::Entity{-1};
    reg::Entity cameraEntity = reg::Entity{-1};
    Shader *currentShader = nullptr;

    CubeBatch cubeBatch;

    // replaced by m_textBatch.fontAtlas
    // const Texture *m_fontAtlasTexture = nullptr;
  };

  std::map<MaterialKey, CubeBatch> m_cubeBatchCache = {};
  M m;
  template <typename NRVO>
    requires std::same_as<std::invoke_result_t<NRVO>, M>
  Renderer3d(NRVO &&factory) : m(factory()){};
  friend class Application;
};

} // namespace pain
