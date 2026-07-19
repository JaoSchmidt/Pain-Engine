/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <array>
#include <cstdint>
#include <glm/glm.hpp>

// NOTE: make sure this file never gets a dependency for this engine. It's
// suppose to be very simple
namespace pain
{
/**
 * @enum RenderPass
 * @brief For ordering logical passes between render functions
 */
enum class RenderPass : uint8_t {
  Dim3d,
  Dim2d,
  Script,
  UI,
  Count,
};

#define RENDER_LAYER_ENUM(X)                                                   \
  X(A) /* Very distant */                                                      \
  X(B) /* Distant */                                                           \
  X(C) /* Background */                                                        \
  X(D) /* Default */                                                           \
  X(E) /* Foreground */                                                        \
  X(F) /* Close */                                                             \
  X(G) /* Very Close */

/**
 * @enum RenderLayer
 * @brief Logical depth ordering used by the renderer.
 *
 * Render layers define **draw order priority**. Higher values are rendered
 * closer to the camera and typically appear on top of lower layers.
 *
 * ### 🎯 Usage
 * - Sorting renderables before submission.
 * - Layer-based visibility control.
 * - Simple depth abstraction without full Z-buffer reliance.
 */
enum class RenderLayer : uint8_t {
#define X(name) name,
  RENDER_LAYER_ENUM(X)
#undef X
};

/** Total number of supported render layers. */
static constexpr uint8_t NumLayers = 7;

/**
 * @enum SphereDivision
 * @brief Pre defined divisions on the sphere polygon
 */
enum class SphereDivision : uint8_t { D_8x8, D_16x16, D_32x32, Count };
/** @brief (slices, stacks) value of each sphere division */
constexpr glm::ivec2 s_resolutions[] = {{8, 8}, {16, 16}, {32, 32}};
/** @brief return the (slices, stacks) for each sphere division */
constexpr glm::uvec2 getResolution(SphereDivision r)
{
  return s_resolutions[static_cast<uint32_t>(r)];
}
constexpr std::array<SphereDivision, 3> s_sphereDivisions = {
    SphereDivision::D_8x8, SphereDivision::D_16x16, SphereDivision::D_32x32};

/** Render sumbition types used during scripts to communicate with the renderer
 * Modifying this requires a modification of RenderContext.h and RenderSys.h
 */
enum class RenderCommandType : uint8_t {
  Sphere,
  Cube,
  Quad,
  Rect,
  Line,
  Circle,
  Triangle,
  DebugLine,
  LightPoint,
  Text,
  Count
};

/** Simple geometry types to be used with the mesh */
enum MeshShape {
  Shpere_8x8,   //
  Shpere_16x16, //
  Shpere_32x32, //
  Cube,         //
  Count         // Other flexible shapes soon
};

/** @defgroup Rendering Rendering Core */

} // namespace pain
