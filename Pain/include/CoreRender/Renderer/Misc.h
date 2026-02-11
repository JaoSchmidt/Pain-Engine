/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <cstdint>
#include <glm/glm.hpp>

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

  /**
   * @brief Creates a color from a packed RGBA integer.
   *
   * @param rgba Packed 32-bit color value.
   */
  constexpr explicit Color(uint32_t rgba) : value(rgba) {}

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
      : value((uint32_t)a << 24 |
              (uint32_t)b << 16 |
              (uint32_t)g << 8  |
              (uint32_t)r)
  {}

  // clang-format on
};

namespace Colors
{
// Neutrals
static constexpr Color LightGrey{204, 204, 204, 255};
static constexpr Color Grey{128, 128, 128, 255};
static constexpr Color DarkerGrey{32, 32, 32, 255};
static constexpr Color OffWhite{245, 245, 245, 255};

// Reds / Pinks
static constexpr Color Red{255, 0, 0, 255};
static constexpr Color DarkRed{139, 0, 0, 255};
static constexpr Color SoftPink{255, 182, 193, 255};
static constexpr Color Magenta{255, 0, 255, 255};

// Oranges / Yellows
static constexpr Color Orange{255, 165, 0, 255};
static constexpr Color DarkOrange{255, 140, 0, 255};
static constexpr Color Gold{255, 215, 0, 255};
static constexpr Color Yellow{255, 255, 0, 255};

// Greens
static constexpr Color Green{0, 255, 0, 255};
static constexpr Color DarkGreen{0, 100, 0, 255};
static constexpr Color Lime{50, 205, 50, 255};
static constexpr Color Olive{128, 128, 0, 255};

// Blues
static constexpr Color Blue{0, 0, 255, 255};
static constexpr Color SkyBlue{135, 206, 235, 255};
static constexpr Color DodgerBlue{30, 144, 255, 255};
static constexpr Color Navy{0, 0, 128, 255};

// Purples
static constexpr Color Purple{128, 0, 128, 255};
static constexpr Color Violet{238, 130, 238, 255};
static constexpr Color Indigo{75, 0, 130, 255};

// Browns
static constexpr Color Brown{139, 69, 19, 255};
static constexpr Color SaddleBrown{160, 82, 45, 255};

// Special / Utility
static constexpr Color Cyan{0, 255, 255, 255};
static constexpr Color Teal{0, 128, 128, 255};
static constexpr Color TransparentWhite{255, 255, 255, 128};
static constexpr Color TransparentBlack{0, 0, 0, 128};
} // namespace Colors

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
  Distant = 0,
  FurtherBack = 1,
  Background = 2,
  Default = 3,
  Closer = 4,
  MuchCloser = 5,
  TouchingCamera = 6,
};

/**
 * @enum SphereDivision
 * @brief Pre defined divisions on the sphere polygon
 */
enum class SphereDivision : uint8_t { D_8x8, D_16x16, D_32x32, Count };

constexpr glm::ivec2 resolutions[] = {{8, 8}, {16, 16}, {32, 32}};

constexpr glm::ivec2 getResolution(SphereDivision r)
{
  return resolutions[static_cast<int>(r)];
}

/** Total number of supported render layers. */
static constexpr uint8_t NumLayers = 7;
/** @defgroup Rendering Rendering Core */

} // namespace pain
