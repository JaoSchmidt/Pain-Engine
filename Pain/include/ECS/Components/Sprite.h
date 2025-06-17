#pragma once

#include "CoreFiles/ResourceManagerSing.h"
#include "CoreRender/Texture.h"

namespace pain
{
struct SpriteComponent {
  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  float m_tilingFactor = 1.f;
  Texture *m_texture = &resources::getDefaultTexture(resources::ERROR);
  void setTexture(Texture &texture) { m_texture = &texture; };
  Texture &getTexture() const { return *m_texture; };

  // Constructors
  SpriteComponent(const glm::vec2 &size, const glm::vec4 &color,
                  float tilingFactor, Texture &texture)
      : m_size(size), m_color(color), m_tilingFactor(tilingFactor),
        m_texture(&texture)
  {
  }
  SpriteComponent() = default;

  // Move
  SpriteComponent(SpriteComponent &&other) noexcept
      : m_size(std::move(other.m_size)), m_color(std::move(other.m_color)),
        m_tilingFactor(other.m_tilingFactor), m_texture(other.m_texture)
  {
  }
  SpriteComponent &operator=(SpriteComponent &&other) noexcept
  {
    if (this != &other) {
      m_size = std::move(other.m_size);
      m_color = std::move(other.m_color);
      m_tilingFactor = other.m_tilingFactor;
      m_texture = other.m_texture;
    }
    return *this;
  }
};

struct SpritelessComponent {
  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  SpritelessComponent() = default;
  SpritelessComponent(const glm::vec2 &size, const glm::vec4 &color)
      : m_size(size), m_color(color)
  {
  }
};

struct TrianguleComponent {
  glm::vec2 m_height{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  TrianguleComponent() = default;
  TrianguleComponent(const glm::vec2 &height, const glm::vec4 &color)
      : m_height(height), m_color(color)
  {
  }
};

} // namespace pain
