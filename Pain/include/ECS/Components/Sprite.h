#pragma once

#include "pch.h"

namespace pain
{
struct SpriteRendererComponent {
  glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

  SpriteRendererComponent() = default;
  SpriteRendererComponent(const SpriteRendererComponent &) = default;
  SpriteRendererComponent(const glm::vec4 &color) : Color(color) {}
};

struct KeyinputComponent {
};

} // namespace pain
