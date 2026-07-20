/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file TextComponent.h
 * @brief Renderable ECS components for text.
 */

#pragma once

#include "CoreRender/Renderer/Colors.h"
#include "ECS/Components/ComponentManager.h"
#include "Font.h"
#include <aliases.h>

namespace pain
{

enum class TextAlign : uint8_t { Left, Right };

/**
 * @brief ECS component representing text.
 */
struct TextComponent {
  using tag = tag::TextComponent;
  static TextComponent create(Font &font) { return {.font = font}; }
  std::string text = "undefined";
  Color color = Colors::Black;
  float scale = 10.f;
  TextAlign align = TextAlign::Left;
  aRefWrap<Font> font;
};

} // namespace pain
