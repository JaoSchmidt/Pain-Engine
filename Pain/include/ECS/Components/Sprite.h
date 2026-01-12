#pragma once

#include "Assets/DefaultTexture.h"
#include "CoreRender/Texture.h"
#include "Misc/BasicShape.h"
#include "Misc/TextureSheet.h"

#include <variant>

namespace pain
{
struct SheetStruct {
  TextureSheet *sheet;
  unsigned short id;
};

struct SpriteComponent {
  using tag = tag::Sprite;
  using TextureVariant = std::variant<Texture *, SheetStruct>;

  glm::vec2 m_size{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  float m_tilingFactor = 1.f;
  TextureVariant m_tex = TextureVariant{
      &resources::getDefaultTexture(resources::DefaultTexture::Error, false)};
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
  //  Factory functions
  // ------------------------------------------------------------

  static SpriteComponent create(glm::vec2 size = {0.1f, 0.1f})
  {
    return SpriteComponent{.m_size = size,
                           .m_tex = &resources::getDefaultTexture(
                               resources::DefaultTexture::General, false)};
  }
  // -- Texture by file path
  static SpriteComponent create(const char *textureFilePath,
                                glm::vec2 size = {0.1f, 0.1f})
  {
    return SpriteComponent{.m_size = size,
                           .m_tex = &resources::getTexture(textureFilePath)};
  }
  static SpriteComponent create(const char *sheetFilePath, unsigned short id,
                                glm::vec2 size = {0.1f, 0.1f})
  {
    return SpriteComponent{
        .m_size = size,
        .m_tex = SheetStruct{&resources::getTextureSheet(sheetFilePath), id}};
  }

  // -- Texture by reference
  static SpriteComponent create(Texture &texture, glm::vec2 size = {0.1f, 0.1f})
  {
    return SpriteComponent{.m_size = size, .m_tex = &texture};
  }
  static SpriteComponent create(TextureSheet &sheet, unsigned short id,
                                glm::vec2 size = {0.1f, 0.1f})
  {
    return SpriteComponent{.m_size = size, .m_tex = SheetStruct{&sheet, id}};
  }
  static SpriteComponent create(Texture &texture, const glm::vec2 &size,
                                const glm::vec4 &color,
                                float tilingFactor = 1.f)
  {
    return SpriteComponent{.m_size = size,
                           .m_color = color,
                           .m_tilingFactor = tilingFactor,
                           .m_tex = &texture};
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
};

struct SpritelessComponent {
  using tag = tag::Spriteless;
  std::variant<CircleShape, QuadShape> m_shape = QuadShape();
  glm::vec4 m_color{0.8f, 0.2f, 0.1f, 0.8f};
  static SpritelessComponent createQuad(const glm::vec2 &size)
  {
    return SpritelessComponent{.m_shape = QuadShape(size)};
  }
  static SpritelessComponent createCircle(float radius)
  {
    return SpritelessComponent{.m_shape = CircleShape(radius)};
  }
  static SpritelessComponent createQuad(const glm::vec2 &size,
                                        const glm::vec4 &color)
  {
    return SpritelessComponent{.m_shape = QuadShape(size), .m_color = color};
  }
  static SpritelessComponent createCircle(float radius, const glm::vec4 &color)
  {
    return SpritelessComponent{.m_shape = CircleShape(radius),
                               .m_color = color};
  }
};

struct TrianguleComponent {
  using tag = tag::Triangule;
  glm::vec2 m_height{0.1f, 0.1f};
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  static TrianguleComponent create(const glm::vec2 &height)
  {
    return TrianguleComponent{.m_height = height};
  }
  static TrianguleComponent create(const glm::vec2 &height,
                                   const glm::vec4 &color)
  {
    return TrianguleComponent{.m_height = height, .m_color = color};
  }
};

} // namespace pain
