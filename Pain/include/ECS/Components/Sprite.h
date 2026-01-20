/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file SpriteComponent.h
 * @brief Renderable ECS components for textured and primitive sprites.
 *
 * Defines components used by the rendering systems to draw textured sprites,
 * texture-sheet sprites, and simple geometric primitives.
 *
 * These components are data-only and contain no rendering logic. They describe
 * visual state such as size, color, texture source, and render layer.
 */

#pragma once

#include "Assets/ManagerTexture.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/ComponentManager.h"
#include "Misc/BasicShape.h"
#include "Misc/TextureSheet.h"

#include <variant>

namespace pain
{

/**
 * @brief Reference to a texture sheet entry.
 *
 * Stores a pointer to a texture sheet and an index identifying the sub-texture
 * inside the sheet.
 */
struct SheetStruct {
  TextureSheet *sheet; /**< Texture sheet reference. */
  unsigned short id;   /**< Sub-texture index inside the sheet. */
};

/**
 * @brief Configuration used when creating a SpriteComponent.
 *
 * Acts as a lightweight descriptor to initialize a sprite with common visual
 * properties without directly touching the component fields.
 */
struct SpriteCreationInfo {
  glm::vec2 m_size{0.1f, 0.1f};             /**< Sprite size in world units. */
  Color color = {255, 255, 255, 255};       /**< Tint color. */
  float m_tilingFactor = 1.f;               /**< Texture tiling multiplier. */
  RenderLayer layer = RenderLayer::Default; /**< Rendering order layer. */
};

/**
 * @brief ECS component representing a textured sprite.
 *
 * A sprite may reference either:
 *  - A single texture.
 *  - A texture sheet entry.
 *
 * The active texture source is stored as a variant and accessed through
 * type-safe getters.
 */
struct SpriteComponent {
  using tag = tag::Sprite;

  /** @brief Variant type representing the sprite texture source. */
  using TextureVariant = std::variant<Texture *, SheetStruct>;

  glm::vec2 m_size{0.1f, 0.1f};             /**< Sprite size in world units. */
  Color color = {255, 255, 255, 255};       /**< Tint color. */
  float m_tilingFactor = 1.f;               /**< Texture tiling multiplier. */
  RenderLayer layer = RenderLayer::Default; /**< Rendering order layer. */

  /**
   * @brief Texture source used by the sprite.
   *
   * Defaults to a fallback texture to ensure safe rendering.
   */
  TextureVariant m_tex = TextureVariant{&TextureManager::getDefaultTexture(
      TextureManager::DefaultTexture::Error)};

  /** @brief Debug utility for printing the active texture pointer. */
  void printptr(const char *label) const
  {
    std::visit(
        [&](auto &&obj) {
          using T = std::decay_t<decltype(obj)>;
          if constexpr (std::is_same_v<T, SheetStruct>) {
            PLOG_I("ptr = {} on {}", fmt::ptr(obj.sheet), label);
          } else {
            PLOG_I("ptr = {} on {}", fmt::ptr(obj), label);
          }
        },
        m_tex);
  }

  // ------------------------------------------------------------
  // Factory functions
  // ------------------------------------------------------------

  /**
   * @brief Creates a sprite using the default texture.
   *
   * @param info Sprite configuration parameters.
   */
  static SpriteComponent create(const SpriteCreationInfo &info = {})
  {
    return SpriteComponent{.m_size = info.m_size,
                           .color = info.color,
                           .m_tilingFactor = info.m_tilingFactor,
                           .layer = info.layer,
                           .m_tex = &TextureManager::getDefaultTexture(
                               TextureManager::DefaultTexture::General, false)};
  }

  /**
   * @brief Creates a sprite from a texture file path.
   *
   * @param info Sprite configuration parameters.
   * @param textureFilePath Path to the texture file.
   */
  static SpriteComponent create(const SpriteCreationInfo &info,
                                const char *textureFilePath)
  {
    return SpriteComponent{.m_size = info.m_size,
                           .color = info.color,
                           .m_tilingFactor = info.m_tilingFactor,
                           .layer = info.layer,
                           .m_tex =
                               &TextureManager::getTexture(textureFilePath)};
  }

  /**
   * @brief Creates a sprite from a texture sheet file and entry index.
   *
   * @param info Sprite configuration parameters.
   * @param sheetFilePath Path to the texture sheet file.
   * @param id Sub-texture index.
   */
  static SpriteComponent create(const SpriteCreationInfo &info,
                                const char *sheetFilePath, unsigned short id)
  {
    return SpriteComponent{
        .m_size = info.m_size,
        .color = info.color,
        .m_tilingFactor = info.m_tilingFactor,
        .layer = info.layer,
        .m_tex =
            SheetStruct{&TextureManager::getTextureSheet(sheetFilePath), id}};
  }

  /**
   * @brief Creates a sprite from an existing texture reference.
   *
   * @param info Sprite configuration parameters.
   * @param texture Texture reference.
   */
  static SpriteComponent create(const SpriteCreationInfo &info,
                                Texture &texture)
  {
    return SpriteComponent{.m_size = info.m_size,
                           .color = info.color,
                           .m_tilingFactor = info.m_tilingFactor,
                           .layer = info.layer,
                           .m_tex = &texture};
  }

  /**
   * @brief Creates a sprite from an existing texture sheet reference.
   *
   * @param info Sprite configuration parameters.
   * @param sheet Texture sheet reference.
   * @param id Sub-texture index.
   */
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
  // Getters
  // ------------------------------------------------------------

  /**
   * @brief Returns texture coordinates for a texture-sheet sprite.
   *
   * @warning Undefined behavior if the sprite is not backed by a texture sheet.
   */
  const std::array<glm::vec2, 4> &getCoords() const
  {
    SheetStruct sheet = std::get<SheetStruct>(m_tex);
    return (*sheet.sheet)[sheet.id];
  }

  /**
   * @brief Returns the underlying texture when using a direct texture.
   *
   * @warning Undefined behavior if the sprite is backed by a texture sheet.
   */
  Texture &getTexture() const { return *std::get<Texture *>(m_tex); }

  /**
   * @brief Returns the texture associated with the active texture sheet.
   *
   * @warning Undefined behavior if the sprite is not backed by a texture sheet.
   */
  Texture &getTextureFromTextureSheet() const
  {
    return std::get<SheetStruct>(m_tex).sheet->getTexture();
  }

  // ------------------------------------------------------------
  // Setters
  // ------------------------------------------------------------

  /** @brief Assigns a direct texture reference. */
  void setTexture(Texture &texture) { m_tex = TextureVariant{&texture}; }

  /** @brief Loads and assigns a texture from a file path. */
  void setTexture(const char *filepath)
  {
    m_tex = TextureVariant{&TextureManager::getTexture(filepath)};
  }

  /** @brief Assigns a texture sheet entry. */
  void setTextureSheet(TextureSheet &sheet, unsigned short id)
  {
    m_tex = TextureVariant{SheetStruct{&sheet, id}};
  }
};

/**
 * @brief ECS component representing a non-textured primitive shape.
 *
 * Used for rendering simple debug or stylized geometry such as quads or circles
 * without requiring a texture.
 */
struct SpritelessComponent {
  using tag = tag::Spriteless;

  std::variant<CircleShape, QuadShape> m_shape =
      QuadShape();                             /**< Shape geometry. */
  Color color = {204, 51, 25, 76};             /**< Fill color. */
  RenderLayer layer = RenderLayer::MuchCloser; /**< Rendering order layer. */

  /** @brief Creates a quad primitive. */
  static SpritelessComponent createQuad(const glm::vec2 &size)
  {
    return SpritelessComponent{.m_shape = QuadShape(size)};
  }

  /** @brief Creates a circle primitive. */
  static SpritelessComponent createCircle(float radius)
  {
    return SpritelessComponent{.m_shape = CircleShape(radius)};
  }

  /** @brief Creates a colored quad primitive. */
  static SpritelessComponent createQuad(const glm::vec2 &size,
                                        const Color &color)
  {
    return SpritelessComponent{.m_shape = QuadShape(size), .color = color};
  }

  /** @brief Creates a colored circle primitive. */
  static SpritelessComponent createCircle(float radius, const Color &color)
  {
    return SpritelessComponent{.m_shape = CircleShape(radius), .color = color};
  }
};

/** @brief ECS component representing a simple triangle primitive. */
struct TrianguleComponent {
  using tag = tag::Triangule;

  glm::vec2 m_height{0.1f, 0.1f};            /**< Triangle dimensions. */
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f}; /**< Vertex color. */

  /** @brief Creates a triangle with default color. */
  static TrianguleComponent create(const glm::vec2 &height)
  {
    return TrianguleComponent{.m_height = height};
  }

  /** @brief Creates a triangle with custom color. */
  static TrianguleComponent create(const glm::vec2 &height,
                                   const glm::vec4 &color)
  {
    return TrianguleComponent{.m_height = height, .m_color = color};
  }
};

} // namespace pain
