/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <cstdint>

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

static constexpr Color White{255, 255, 255, 255};
static constexpr Color Black{0, 0, 0, 255};
static constexpr Color Transparent{0, 0, 0, 0};

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

/** Total number of supported render layers. */
static constexpr uint8_t NumLayers = 7;
/** @defgroup Rendering Rendering Core */

} // namespace pain
