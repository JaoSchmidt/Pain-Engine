#pragma once

#include "Assets/DefaultTexture.h"
#include "CoreRender/Texture.h"
#include "Misc/BasicShape.h"
#include "Misc/TextureSheet.h"

namespace pain
{
struct SheetStruct {
  TextureSheet *sheet;
  unsigned short id;
};

struct SpriteComponent {
  using TextureVariant = std::variant<Texture *, SheetStruct>;

  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  float m_tilingFactor = 1.f;
  TextureVariant m_tex = TextureVariant{
      &resources::getDefaultTexture(resources::DEFAULT_TEXTURE::ERROR, false)};

  void printptr(const char *a) const
  {
    std::visit(
        [&](auto &&obj) {
          using T = std::decay_t<decltype(obj)>;
          if constexpr (std::is_same_v<T, SheetStruct>) {
            PLOG_I("ptr = {} on {}", fmt::ptr(obj.sheet), a);
          } else {
            PLOG_I("ptr = {} on {}", fmt::ptr(obj), a);
          }
        },
        m_tex);
  }

  // ------------------------------------------------------------
  //  GETTERS with type-enforced stuff
  // ------------------------------------------------------------

  // Assuming sprite is part of a Texture Atlas, get its coordinates
  const std::array<glm::vec2, 4> &getCoords() const
  {
    SheetStruct sheet = std::get<SheetStruct>(m_tex);
    return (*sheet.sheet)[sheet.id];
  }
  Texture &getTexture() const { return *std::get<Texture *>(m_tex); }
  Texture &getTextureFromTextureSheet() const
  {
    return std::get<SheetStruct>(m_tex).sheet->getTexture();
  }

  // ------------------------------------------------------------
  //  SETTERS
  // ------------------------------------------------------------

  // set a single texture
  void setTexture(Texture &texture) { m_tex = TextureVariant{&texture}; }
  void setTexture(const char *filepath)
  {
    m_tex = TextureVariant{&resources::getTexture(filepath)};
  }
  void setTextureSheet(TextureSheet &sheet, unsigned short id)
  {
    m_tex = TextureVariant{SheetStruct{&sheet, id}};
  }

  // --- Constructors ---
  SpriteComponent(const glm::vec2 &size, const glm::vec4 &color,
                  float tilingFactor, Texture &texture)
      : m_size(size), m_color(color), m_tilingFactor(tilingFactor),
        m_tex(&texture) {};

  SpriteComponent(Texture &texture, glm::vec2 size = {0.1f, 0.1f})
      : m_size(size), m_tex(&texture) {};

  SpriteComponent(TextureSheet &sheet, unsigned short id,
                  glm::vec2 size = {0.1f, 0.1f})
      : m_size(size), m_tex(SheetStruct{&sheet, id}) {};

  SpriteComponent(const char *filepath)
      : m_tex(&resources::getTexture(filepath)) {};

  SpriteComponent() = default;

  // --- Move semantics are fully handled by std::variant ---
  SpriteComponent(SpriteComponent &&) noexcept = default;
  SpriteComponent &operator=(SpriteComponent &&) noexcept = default;
};

struct SpritelessComponent {
  std::variant<CircleShape, QuadShape> m_shape = QuadShape();
  glm::vec4 m_color{0.8f, 0.2f, 0.1f, 0.8f};
  SpritelessComponent() = default;
  SpritelessComponent(const glm::vec2 &size) : m_shape(QuadShape(size)) {};
  SpritelessComponent(const float radius) : m_shape(CircleShape(radius)) {};

  SpritelessComponent(const float radius, const glm::vec4 &color)
      : m_shape(CircleShape(radius)), m_color(color) {};
  SpritelessComponent(const glm::vec2 &size, const glm::vec4 &color)
      : m_shape(QuadShape(size)), m_color(color) {};
};

struct TrianguleComponent {
  glm::vec2 m_height{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  TrianguleComponent() = default;
  TrianguleComponent(const glm::vec2 &height, const glm::vec4 &color)
      : m_height(height), m_color(color) {};
};

} // namespace pain
