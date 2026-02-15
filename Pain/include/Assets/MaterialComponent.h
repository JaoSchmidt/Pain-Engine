/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file MaterialComponent.h
 * @brief Renderable ECS components for textured and primitive materials.
 *
 * Defines components used by the rendering systems to draw textured materials,
 * texture-sheet materials, and simple geometric primitives.
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
 * @brief Configuration used when creating a MaterialComponent.
 *
 * Acts as a lightweight descriptor to initialize a material with common visual
 * properties without directly touching the component fields.
 */
struct MaterialCreationInfo {
  Color color = {255, 255, 255, 255}; /**< Tint color. */
  float tilingFactor = 1.f;           /**< Texture tiling multiplier. */
};

/**
 * @brief ECS component representing a textured material.
 *
 * A material may reference either:
 *  - A single texture.
 *  - A texture sheet entry.
 *
 * The active texture source is stored as a variant and accessed through
 * type-safe getters.
 */
struct MaterialComponent {
private:
  struct SheetStruct {
    TextureSheet *sheet; /**< Texture sheet reference. */
    unsigned short id;   /**< Sub-texture index inside the sheet. */
  };

public:
  using tag = tag::Material;

  /** @brief Variant type representing the material texture source. */
  using TextureVariant = std::variant<Texture *, SheetStruct>;

  Color m_color = {255, 255, 255, 255}; /**< Tint color. */
  float m_tilingFactor = 1.f;           /**< Texture tiling multiplier. */

  /**
   * @brief Texture source used by the material.
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
   * @brief Creates a material using the default texture.
   *
   * @param info Material configuration parameters.
   */
  static MaterialComponent create(const MaterialCreationInfo &info = {})
  {
    return MaterialComponent{
        .m_color = info.color,
        .m_tilingFactor = info.tilingFactor,
        .m_tex = &TextureManager::getDefaultTexture(
            TextureManager::DefaultTexture::General, false)};
  }

  /**
   * @brief Creates a material from a texture file path.
   *
   * @param info Material configuration parameters.
   * @param textureFilePath Path to the texture file.
   */
  static MaterialComponent create(const MaterialCreationInfo &info,
                                  const char *textureFilePath)
  {
    return MaterialComponent{.m_color = info.color,
                             .m_tilingFactor = info.tilingFactor,
                             .m_tex =
                                 &TextureManager::getTexture(textureFilePath)};
  }

  /**
   * @brief Creates a material from a texture sheet file and entry index.
   *
   * @param info Material configuration parameters.
   * @param sheetFilePath Path to the texture sheet file.
   * @param id Sub-texture index.
   */
  static MaterialComponent create(const MaterialCreationInfo &info,
                                  const char *sheetFilePath, unsigned short id)
  {
    return MaterialComponent{
        .m_color = info.color,
        .m_tilingFactor = info.tilingFactor,
        .m_tex =
            SheetStruct{&TextureManager::getTextureSheet(sheetFilePath), id}};
  }

  /**
   * @brief Creates a material from an existing texture reference.
   *
   * @param info Material configuration parameters.
   * @param texture Texture reference.
   */
  static MaterialComponent create(const MaterialCreationInfo &info,
                                  Texture &texture)
  {
    return MaterialComponent{.m_color = info.color,
                             .m_tilingFactor = info.tilingFactor,
                             .m_tex = &texture};
  }

  /**
   * @brief Creates a material from an existing texture sheet reference.
   *
   * @param info Material configuration parameters.
   * @param sheet Texture sheet reference.
   * @param id Sub-texture index.
   */
  static MaterialComponent create(const MaterialCreationInfo &info,
                                  TextureSheet &sheet, unsigned short id)
  {
    return MaterialComponent{.m_color = info.color,
                             .m_tilingFactor = info.tilingFactor,
                             .m_tex = SheetStruct{&sheet, id}};
  }

  // ------------------------------------------------------------
  // Getters
  // ------------------------------------------------------------

  /**
   * @brief Returns texture coordinates for a texture-sheet material.
   *
   * @warning Undefined behavior if the material is not backed by a texture
   * sheet.
   */
  const std::array<glm::vec2, 4> &getCoords() const
  {
    SheetStruct sheet = std::get<SheetStruct>(m_tex);
    return (*sheet.sheet)[sheet.id];
  }

  /**
   * @brief Returns the underlying texture when using a direct texture.
   *
   * @warning Undefined behavior if the material is backed by a texture sheet.
   */
  Texture &getTexture() const { return *std::get<Texture *>(m_tex); }

  /**
   * @brief Returns the texture associated with the active texture sheet.
   *
   * @warning Undefined behavior if the material is not backed by a texture
   * sheet.
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

} // namespace pain
