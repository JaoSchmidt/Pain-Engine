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
#include "CoreRender/Renderer/Stats.h"
#include "Physics/Movement3dComponent.h"
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
   * @param perspCamera Perspecitve camera component
   * @param position camera position
   */
  void beginScene(DeltaTime globalTime, const cmp::PerspCamera &perspCamera,
                  const Transform3dComponent &position);

  // @brief Flush all batches and finalize the scene.
  void endScene(const Scene &scene);

  // ================================================================= //
  // Draw Polygons
  // ================================================================= //

  /// @brief submit a cube polyhedron.
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material);
  /// @brief submit a rotated cube polyhedron. Rotation dimensions are in
  /// radians
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material, const glm::vec3 &rotation);
  /// @brief submits cubes directly with transform
  void submitCube(const glm::mat4 &transform, const Material &material);
  /// @brief submit a UV sphere polyhedron.
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material);
  /// @brief submit a rotating UV sphere polyhedron. Rotation dimensions are in
  /// radians
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material, const glm::vec3 &rotation);
  /// @brief submits sphere directly with transform
  void submitUVSphere(const glm::mat4 &transform, SphereDivision div,
                      const Material &material);
  /// @brief a light position to be used by other shaders
  void submitLight(const glm::vec3 &pos, const Color &color);

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
  Stats getSphereStatistics();

private:
  float constexpr smallSpacingOrder(short order) { return order / 1024.f; };
  void flush();
  void uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                           DeltaTime globalTime, const glm::ivec2 &resolution,
                           const glm::vec3 &cameraPos);
  void bindTextures();
  float allocateTextures(Texture &texture);
  ResolvedMaterial resolveMaterial(const MaterialComponent &mat);

  template <typename Batch>
    requires requires(Batch &b) { b.statsCount; }
  Stats getStatistics(Batch &b)
  {
    if constexpr (std::is_same_v<Batch, CubeBatch>) {
      return {"Cubes", b.statsCount, b.statsCount * 8, b.statsCount * 3,
              b.drawCount};
    } else if constexpr (std::is_same_v<Batch, SphereBatch>) {
      return {"Spheres", b.statsCount, b.statsCount * b.m_indicesPerSphere,
              b.statsCount * b.m_verticesPerSphere, b.drawCount};
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
