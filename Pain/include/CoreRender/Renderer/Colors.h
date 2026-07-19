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

  static constexpr Color fromRGB(uint32_t rgb)
  {
    return Color((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF, 255);
  }
  static constexpr Color fromRGBA(uint32_t rgba)
  {
    return Color((rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF,
                 rgba & 0xFF);
  }
};

namespace Colors
{
template <std::size_t N>
static constexpr std::array<Color, N> makeGradient(Color from, Color to)
{
  static_assert(N > 0);

  std::array<Color, N> colors{};

  if constexpr (N == 1) {
    colors[0] = from;
    return colors;
  }

  const uint8_t r0 = from.value & 0xFF;
  const uint8_t g0 = (from.value >> 8) & 0xFF;
  const uint8_t b0 = (from.value >> 16) & 0xFF;
  const uint8_t a0 = (from.value >> 24) & 0xFF;

  const uint8_t r1 = to.value & 0xFF;
  const uint8_t g1 = (to.value >> 8) & 0xFF;
  const uint8_t b1 = (to.value >> 16) & 0xFF;
  const uint8_t a1 = (to.value >> 24) & 0xFF;

  for (std::size_t i = 0; i < N; ++i) {
    const float t = static_cast<float>(i) / static_cast<float>(N - 1);
    const auto lerp = [t](uint8_t a, uint8_t b) -> uint8_t {
      return static_cast<uint8_t>(a + (b - a) * t);
    };
    colors[i] = Color(lerp(r0, r1), lerp(g0, g1), lerp(b0, b1), lerp(a0, a1));
  }

  return colors;
}

static constexpr Color Black = Color::fromRGB(0x000000);
static constexpr Color LightGrey = Color::fromRGB(0xCCCCCC);
static constexpr Color PastelLightGrey = Color::fromRGB(0xE8E8E8);

static constexpr Color Grey = Color::fromRGB(0x808080);
static constexpr Color PastelGrey = Color::fromRGB(0xC8C8C8);

static constexpr Color DarkerGrey = Color::fromRGB(0x202020);
static constexpr Color PastelDarkerGrey = Color::fromRGB(0x8A8A8A);

static constexpr Color OffWhite = Color::fromRGB(0xF5F5F5);
static constexpr Color PastelOffWhite = Color::fromRGB(0xFAFAF8);

static constexpr Color FullWhite = Color::fromRGB(0xFFFFFF);

// Reds / Pinks
static constexpr Color Red = Color::fromRGB(0xFF0000);
static constexpr Color PastelRed = Color::fromRGB(0xFFB3BA);

static constexpr Color DarkRed = Color::fromRGB(0x8B0000);
static constexpr Color PastelDarkRed = Color::fromRGB(0xD88C8C);

static constexpr Color SoftPink = Color::fromRGB(0xFFB6C1);
static constexpr Color PastelSoftPink = Color::fromRGB(0xFFD6E0);

static constexpr Color StrongPink = Color::fromRGB(0xD40B95);
static constexpr Color PastelStrongPink = Color::fromRGB(0xF4A6C8);

static constexpr Color Magenta = Color::fromRGB(0xFF00FF);
static constexpr Color PastelMagenta = Color::fromRGB(0xE7B7F5);

// Oranges / Yellows
static constexpr Color Orange = Color::fromRGB(0xFFA500);
static constexpr Color PastelOrange = Color::fromRGB(0xFFD1A9);

static constexpr Color DarkOrange = Color::fromRGB(0xFF8C00);
static constexpr Color PastelDarkOrange = Color::fromRGB(0xF5BE8A);

static constexpr Color Gold = Color::fromRGB(0xFFD700);
static constexpr Color PastelGold = Color::fromRGB(0xF8E29A);

static constexpr Color Yellow = Color::fromRGB(0xFFFF00);
static constexpr Color PastelYellow = Color::fromRGB(0xFFF7AE);

// Greens
static constexpr Color Green = Color::fromRGB(0x00FF00);
static constexpr Color PastelGreen = Color::fromRGB(0xB8F2B3);

static constexpr Color DarkGreen = Color::fromRGB(0x006400);
static constexpr Color PastelDarkGreen = Color::fromRGB(0x9AD6A0);

static constexpr Color Lime = Color::fromRGB(0x32CD32);
static constexpr Color PastelLime = Color::fromRGB(0xD8F5A2);

static constexpr Color Olive = Color::fromRGB(0x808000);
static constexpr Color PastelOlive = Color::fromRGB(0xC9D39A);

// Blues
static constexpr Color Blue = Color::fromRGB(0x0000FF);
static constexpr Color PastelBlue = Color::fromRGB(0xAECBFA);

static constexpr Color SkyBlue = Color::fromRGB(0x87CEEB);
static constexpr Color PastelSkyBlue = Color::fromRGB(0xCBEFFF);

static constexpr Color DodgerBlue = Color::fromRGB(0x1E90FF);
static constexpr Color PastelDodgerBlue = Color::fromRGB(0xA9D8FF);

static constexpr Color Navy = Color::fromRGB(0x000080);
static constexpr Color PastelNavy = Color::fromRGB(0xA6B7E8);

// Purples
static constexpr Color Purple = Color::fromRGB(0x800080);
static constexpr Color PastelPurple = Color::fromRGB(0xD7B6F5);

static constexpr Color Violet = Color::fromRGB(0xEE82EE);
static constexpr Color PastelViolet = Color::fromRGB(0xE9CCF7);

static constexpr Color Indigo = Color::fromRGB(0x4B0082);
static constexpr Color PastelIndigo = Color::fromRGB(0xBEB4E9);

// Browns
static constexpr Color Brown = Color::fromRGB(0x8B4513);
static constexpr Color PastelBrown = Color::fromRGB(0xD6B29A);

static constexpr Color SaddleBrown = Color::fromRGB(0xA0522D);
static constexpr Color PastelSaddleBrown = Color::fromRGB(0xCFA98D);

// Special / Utility
static constexpr Color Cyan = Color::fromRGB(0x00FFFF);
static constexpr Color PastelCyan = Color::fromRGB(0xBDF6F7);

static constexpr Color Teal = Color::fromRGB(0x008080);
static constexpr Color PastelTeal = Color::fromRGB(0xA9DDD6);
static constexpr Color TransparentWhite = Color::fromRGBA(0xFFFFFF80); //
static constexpr Color TransparentBlack = Color::fromRGBA(0x00000080);
static constexpr Color FullTransparent = Color::fromRGBA(0x00000000);
} // namespace Colors

} // namespace pain
