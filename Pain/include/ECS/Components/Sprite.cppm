module;
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

export module pain.Sprite;
import pain.Texture;
import <utility>;

export namespace pain
{
struct SpriteComponent {
  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  float m_tilingFactor = 1.f;
  Texture *m_ptexture = nullptr;
  SpriteComponent(const glm::vec2 &size, const glm::vec4 &color,
                  float tilingFactor, Texture *ptexture)
      : m_size(size), m_color(color), m_tilingFactor(tilingFactor),
        m_ptexture(ptexture)
  {
  }
  SpriteComponent(SpriteComponent &&other) noexcept
      : m_size(std::move(other.m_size)), m_color(std::move(other.m_color)),
        m_tilingFactor(other.m_tilingFactor), m_ptexture(other.m_ptexture)
  {
    other.m_ptexture = nullptr;
  }

  SpriteComponent &operator=(SpriteComponent &&other) noexcept
  {
    if (this != &other) {
      m_size = std::move(other.m_size);
      m_color = std::move(other.m_color);
      m_tilingFactor = other.m_tilingFactor;
      m_ptexture = other.m_ptexture;
      other.m_ptexture = nullptr;
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
