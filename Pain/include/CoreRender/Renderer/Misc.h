/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include <cstdint>

namespace pain
{

struct Color {
  uint32_t value = 0;

  constexpr Color() = default;

  constexpr explicit Color(uint32_t rgba) : value(rgba) {}
  // clang-format off
  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
      : value((uint32_t)a << 24 |
              (uint32_t)b << 16 |
              (uint32_t)g << 8  |
              (uint32_t)r)
  {};

  // clang-format on
};

enum class RenderLayer : uint8_t {
  Distant = 0,
  FurtherBack = 1,
  Background = 2,
  Default = 3,
  Closer = 4,
  MuchCloser = 5,
  TouchingCamera = 6,
};
static constexpr uint8_t NumLayers = 7;

} // namespace pain
