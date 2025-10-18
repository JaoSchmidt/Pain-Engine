#pragma once

#include "Assets/DefaultTexture.h"
#include "CoreRender/Texture.h"
#include "Misc/TextureSheet.h"

namespace pain
{
struct SpriteComponent {
  // clang-format off
  glm::vec2 m_size{0.1f, 0.1f};

  glm::vec4 m_color{1.0f, 1.0f, 1.0f,1.0f}; // HACK: perhaps I could map colors?
  float m_tilingFactor = 1.f; // HACK: perhaps this could be inside every texture? or perhaps I could also map this?
  short m_textureSheetId = -1; // check
  union {
    TextureSheet *m_textureSheet;
    Texture *m_texture = &resources::getDefaultTexture(resources::ERROR, false);
  };
  // clang-format on
  void printptr(const char *a) const
  {
    PLOG_I("texsheet id = {} on {}", fmt::ptr(m_textureSheet), a);
  };
  void setTexture(const char *filepath)
  {
    m_texture = &resources::getTexture(filepath);
  };
  const std::array<glm::vec2, 4> &getCoords() const
  {
    return (*m_textureSheet)[m_textureSheetId];
  }
  Texture &getTexture() const
  {
    P_ASSERT_W(m_textureSheetId == -1,
               "You are trying to call getTexture of an atlas. Try calling"
               "getSheetTexture instead");
    return *m_texture;
  };
  Texture &getTextureFromTextureSheet() const
  {
    P_ASSERT_W(
        m_textureSheetId != -1,
        "You are trying to call getTextureSheet of an object that doesn't "
        "have a defined index inside that textureSheet."
        "Perhaps you need to call getTexture instead?");
    return m_textureSheet->getTexture();
  };
  void setTextureSheet(TextureSheet &textureSheet, int id)
  {
    m_textureSheetId = id;
    m_textureSheet = &textureSheet;
  }

  // Constructors
  SpriteComponent(const glm::vec2 &size, const glm::vec4 &color,
                  float tilingFactor, Texture &texture)
      : m_size(size), m_color(color), m_tilingFactor(tilingFactor),
        m_texture(&texture) {};
  SpriteComponent(Texture &texture, glm::vec2 size = {0.1f, 0.1f})
      : m_size(size), m_texture(&texture) {};
  SpriteComponent(TextureSheet &texSheet, short id,
                  glm::vec2 size = {0.1f, 0.1f})
      : m_size(size), m_textureSheetId(id), m_textureSheet(&texSheet) {};
  SpriteComponent(const char *filePath)
      : m_texture(&resources::getTexture(filePath)) {};
  SpriteComponent() = default;

  // Move
  SpriteComponent(SpriteComponent &&other) noexcept
      : m_size(std::move(other.m_size)), m_color(std::move(other.m_color)),
        m_tilingFactor(other.m_tilingFactor),
        m_textureSheetId(other.m_textureSheetId)
  {
    if (m_textureSheetId == -1) {
      m_texture = other.m_texture;
      other.m_texture = nullptr;
    } else {
      m_textureSheet = other.m_textureSheet;
      other.m_textureSheet = nullptr;
    }
  }

  // Move assignment
  SpriteComponent &operator=(SpriteComponent &&other) noexcept
  {
    if (this != &other) {
      m_size = std::move(other.m_size);
      m_color = std::move(other.m_color);
      m_tilingFactor = other.m_tilingFactor;
      m_textureSheetId = other.m_textureSheetId;

      if (m_textureSheetId == -1)
        m_texture = other.m_texture;
      else
        m_textureSheet = other.m_textureSheet;
    }
    return *this;
  }
};

struct SpritelessComponent {
  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  SpritelessComponent() = default;
  SpritelessComponent(const glm::vec2 &size, const glm::vec4 &color)
      : m_size(size), m_color(color) {};
};

struct TrianguleComponent {
  glm::vec2 m_height{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  TrianguleComponent() = default;
  TrianguleComponent(const glm::vec2 &height, const glm::vec4 &color)
      : m_height(height), m_color(color) {};
};

} // namespace pain
