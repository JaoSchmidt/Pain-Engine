#pragma once

#include "Assets/DefaultTexture.h"
#include "CoreRender/Renderer/Misc.h"
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

struct SpriteCreationInfo {
  glm::vec2 m_size{0.1f, 0.1f};
  Color color = {255, 255, 255, 255};
  float m_tilingFactor = 1.f;
  RenderLayer layer = RenderLayer::Default;
};

struct SpriteComponent {
  using tag = tag::Sprite;
  using TextureVariant = std::variant<Texture *, SheetStruct>;

  glm::vec2 m_size{0.1f, 0.1f};
  Color color = {255, 255, 255, 255};
  float m_tilingFactor = 1.f;
  RenderLayer layer = RenderLayer::Default;
  TextureVariant m_tex = TextureVariant{
      &resources::getDefaultTexture(resources::DefaultTexture::Error)};
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

  static SpriteComponent create(const SpriteCreationInfo &info = {})
  {
    return SpriteComponent{.m_size = info.m_size,
                           .color = info.color,
                           .m_tilingFactor = info.m_tilingFactor,
                           .layer = info.layer,
                           .m_tex = &resources::getDefaultTexture(
                               resources::DefaultTexture::General, false)};
  }

  //  Texture by file path
  static SpriteComponent create(const SpriteCreationInfo &info,
                                const char *textureFilePath)
  {
    return SpriteComponent{.m_size = info.m_size,
                           .color = info.color,
                           .m_tilingFactor = info.m_tilingFactor,
                           .layer = info.layer,
                           .m_tex = &resources::getTexture(textureFilePath)};
  }

  static SpriteComponent create(const SpriteCreationInfo &info,
                                const char *sheetFilePath, unsigned short id)
  {
    return SpriteComponent{
        .m_size = info.m_size,
        .color = info.color,
        .m_tilingFactor = info.m_tilingFactor,
        .layer = info.layer,
        .m_tex = SheetStruct{&resources::getTextureSheet(sheetFilePath), id}};
  }

  //  Texture by reference
  static SpriteComponent create(const SpriteCreationInfo &info,
                                Texture &texture)
  {
    return SpriteComponent{.m_size = info.m_size,
                           .color = info.color,
                           .m_tilingFactor = info.m_tilingFactor,
                           .layer = info.layer,
                           .m_tex = &texture};
  }

  static SpriteComponent create(const SpriteCreationInfo &info,
                                TextureSheet &sheet, unsigned short id)
  {
    return SpriteComponent{.m_size = info.m_size,
                           .color = info.color,
                           .m_tilingFactor = info.m_tilingFactor,
                           .layer = info.layer,
                           .m_tex = SheetStruct{&sheet, id}};
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
  Color color = {204, 51, 25, 76};
  RenderLayer layer = RenderLayer::MuchCloser;
  static SpritelessComponent createQuad(const glm::vec2 &size)
  {
    return SpritelessComponent{.m_shape = QuadShape(size)};
  }
  static SpritelessComponent createCircle(float radius)
  {
    return SpritelessComponent{.m_shape = CircleShape(radius)};
  }
  static SpritelessComponent createQuad(const glm::vec2 &size,
                                        const Color &color)
  {
    return SpritelessComponent{.m_shape = QuadShape(size), .color = color};
  }
  static SpritelessComponent createCircle(float radius, const Color &color)
  {
    return SpritelessComponent{.m_shape = CircleShape(radius), .color = color};
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
