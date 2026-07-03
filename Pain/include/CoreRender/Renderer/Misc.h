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
 * @struct Color
 * @brief Compact RGBA color representation used throughout the renderer.
 *
 * Stores color as a single 32-bit unsigned integer in the following layout:
 *
 * ```
 * [ AA | BB | GG | RR ]
 *   31..24 23..16 15..8 7..0
 * ```
 *
 * ### ✨ Notes
 * - Channels are packed as **RGBA** in little-endian order.
 * - Alpha defaults to fully opaque (255).
 * - Designed for **fast copying and GPU uploads**.
 * - Frequently used in draw calls and batch rendering.
 */
struct Color {
  /** Packed RGBA value. */
  uint32_t value = 0;

  /** Creates a default color (all channels set to zero). */
  constexpr Color() = default;

  // clang-format off

  /**
   * @brief Creates a color from individual channel values.
   *
   * @param r Red channel   (0–255)
   * @param g Green channel (0–255)
   * @param b Blue channel  (0–255)
   * @param a Alpha channel (0–255, defaults to 255)
   */
  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
      : value(static_cast<uint32_t>(a) << 24 |
              static_cast<uint32_t>(b) << 16 |
              static_cast<uint32_t>(g) << 8  |
              static_cast<uint32_t>(r))
  {}

  // clang-format on
  constexpr glm::vec4 getVector() const
  {
    return glm::vec4(static_cast<float>(value & 0xFF) / 255.0f,         // r
                     static_cast<float>((value >> 8) & 0xFF) / 255.0f,  // g
                     static_cast<float>((value >> 16) & 0xFF) / 255.0f, // b
                     static_cast<float>((value >> 24) & 0xFF) / 255.0f  // a
    );
  }
};

static constexpr Color fromRGB(uint32_t rgb)
{
  return Color((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF, 255);
}
static constexpr Color fromRGBA(uint32_t rgba)
{
  return Color((rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF,
               rgba & 0xFF);
}

namespace Colors
{
// Neutrals
static constexpr Color LightGrey = fromRGB(0xCCCCCC);  //
static constexpr Color Grey = fromRGB(0x808080);       //
static constexpr Color DarkerGrey = fromRGB(0x202020); //
static constexpr Color OffWhite = fromRGB(0xF5F5F5);   //
static constexpr Color FullWhite = fromRGB(0xFFFFFF);  //

// Reds / Pinks
static constexpr Color Red = fromRGB(0xFF0000);        //
static constexpr Color DarkRed = fromRGB(0x8B0000);    //
static constexpr Color SoftPink = fromRGB(0xFFB6C1);   //
static constexpr Color StrongPink = fromRGB(0xD40B95); //
static constexpr Color Magenta = fromRGB(0xFF00FF);    //

// Oranges / Yellows
static constexpr Color Orange = fromRGB(0xFFA500);     //
static constexpr Color DarkOrange = fromRGB(0xFF8C00); //
static constexpr Color Gold = fromRGB(0xFFD700);       //
static constexpr Color Yellow = fromRGB(0xFFFF00);     //

// Greens
static constexpr Color Green = fromRGB(0x00FF00);     //
static constexpr Color DarkGreen = fromRGB(0x006400); //
static constexpr Color Lime = fromRGB(0x32CD32);      //
static constexpr Color Olive = fromRGB(0x808000);     //
//
// Blues
static constexpr Color Blue = fromRGB(0x0000FF);       //
static constexpr Color SkyBlue = fromRGB(0x87CEEB);    //
static constexpr Color DodgerBlue = fromRGB(0x1E90FF); //
static constexpr Color Navy = fromRGB(0x000080);       //

// Purples
static constexpr Color Purple = fromRGB(0x800080); //
static constexpr Color Violet = fromRGB(0xEE82EE); //
static constexpr Color Indigo = fromRGB(0x4B0082); //

// Browns
static constexpr Color Brown = fromRGB(0x8B4513);       //
static constexpr Color SaddleBrown = fromRGB(0xA0522D); //

// Special / Utility
static constexpr Color Cyan = fromRGB(0x00FFFF);                //
static constexpr Color Teal = fromRGB(0x008080);                //
static constexpr Color TransparentWhite = fromRGBA(0xFFFFFF80); //
static constexpr Color TransparentBlack = fromRGBA(0x00000080);
} // namespace Colors

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
