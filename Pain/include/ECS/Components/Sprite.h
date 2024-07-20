#pragma once

#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Texture.h"

namespace pain
{
struct SpriteComponent {
  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  std::shared_ptr<Texture> m_ptexture;
  const float m_tilingFactor;
  SpriteComponent(const glm::vec2 &size, const glm::vec4 &color,
                  float tilingFactor, std::shared_ptr<Texture> ptexture)
      : m_size(size), m_color(color), m_ptexture(ptexture),
        m_tilingFactor(tilingFactor)
  {
  }
};

struct SpritelessComponent {
  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  SpritelessComponent(const glm::vec2 &size, const glm::vec4 &color)
      : m_size(size), m_color(color)
  {
  }
};

} // namespace pain
