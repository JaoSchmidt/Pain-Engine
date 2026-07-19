/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file Material.h
 * @brief Renderable ECS components for textured and primitive materials.
 *
 * Defines components used by the rendering systems to draw textured materials,
 * texture-sheet materials, and simple geometric primitives.
 *
 * These components are data-only and contain no rendering logic. They describe
 * visual state such as color, texture source, reaction to light, etc.
 */

#pragma once

#include "Assets/ManagerTexture.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Renderer/Colors.h"
#include "platform/ContextBackend.h"

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

struct ParamPBR {         /// Physically Based Rendering
  float roughness = 1.0f; ///< 1 = matte
  float metallic = 0.0f;  ///< 0 = dielectric
  float emission = 0.0f;  ///< Light emission
  auto operator<=>(const ParamPBR &) const = default;
};
struct ParamPhong {       /// Phong only parameters
  float ambient = 1.0f;   ///< 1 = full lit
  float highlight = 0.2f; ///< 1 = light source dominates
  float diffuse = 0.3f;   ///< 1 = Shadow
  auto operator<=>(const ParamPhong &) const = default;
};

/**
 * @brief Configuration used when creating a MaterialComponent.
 *
 * Acts as a lightweight descriptor to initialize a material with common visual
 * properties without directly touching the component fields.
 */
struct MaterialCreationInfo {
  Color color = {255, 255, 255, 255}; ///< Tint color.
  float tilingFactor = 1.f;           ///< Texture tiling multiplier.
  std::variant<ParamPBR, ParamPhong, std::monostate> params = std::monostate{};
  Shader &shader;
  Texture &texture =
      TextureManager::getDefaultTexture(TextureManager::DefaultTexture::Blank);
  std::string name = "";
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
struct Material {
  using TextureVariant = std::variant<Texture *, SheetStruct>;
  std::variant<ParamPBR, ParamPhong, std::monostate>
      m_params;                       ///< Shader specific parameters
  Color m_color = Colors::StrongPink; ///< Tint color.
  float m_tilingFactor = 1.f;         ///< Texture tiling multiplier.
  Shader *m_shader = nullptr;         ///< Instanciated Shader

  uint32_t m_flags = 0; ///< FlagOptions
  enum FlagOptions {
    Unlit = 1 << 0,       //
    Transparent = 1 << 1, //
    DoubleSided = 1 << 2  //
  };

  /** @brief Either Texture or TextureSheet source used by the material.
   * Defaults to a fallback texture to ensure safe rendering, but you should
   * define manually. Otherwise will generate an warning */
  TextureVariant m_texture = TextureVariant{&TextureManager::getDefaultTexture(
      TextureManager::DefaultTexture::Blank, false)};
  std::string m_name = "";

  /** @brief small checker, used mainly on asserts */
  bool isTextureSheet() const
  {
    return std::holds_alternative<SheetStruct>(m_texture);
  }
  // ------------------------------------------------------------
  // Factory functions
  // ------------------------------------------------------------

  /**
   * @brief Creates a material using the initializer.
   *
   * @param info Material configuration parameters.
   */
  static Material create(const MaterialCreationInfo &info)
  {
    int *samplers = new int[backend::getTMU()];
    for (int i = 0; i < backend::getTMUi(); i++)
      samplers[i] = i;

    info.shader.bind();
    info.shader.uploadUniformIntArray("u_Textures", samplers, backend::getTMU(),
                                      false);
    delete[] samplers;

    return Material{
        //
        .m_params = info.params,
        .m_color = info.color,
        .m_tilingFactor = info.tilingFactor,
        .m_shader = &info.shader,
        .m_texture = &info.texture,
        .m_name = std::move(info.name),
    };
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
    SheetStruct sheet = std::get<SheetStruct>(m_texture);
    return (*sheet.sheet)[sheet.id];
  }
  /**
   * @brief Returns the underlying texture when using a direct texture.
   *
   * @warning Undefined behavior if the sprite is backed by a texture sheet.
   */
  Texture &getTexture() const { return *std::get<Texture *>(m_texture); }

  /**
   * @brief Returns the texture associated with the active texture sheet.
   *
   * @warning Undefined behavior if the sprite is not backed by a texture sheet.
   */
  Texture &getTextureFromTextureSheet() const
  {
    return std::get<SheetStruct>(m_texture).sheet->getTexture();
  }
  // ------------------------------------------------------------
  // Setters
  // ------------------------------------------------------------

  /** @brief Assigns a direct texture reference. */
  void setTexture(Texture &texture)
  {
    P_ASSERT_W(!isTextureSheet(),
               "You are defining a TextureSheet in a material that originally "
               "used plain Textures");
    m_texture = TextureVariant{&texture};
  }

  /** @brief Loads and assigns a texture from a file path. */
  void setTexture(const char *filepath)
  {
    P_ASSERT_W(!isTextureSheet(),
               "You are defining a TextureSheet in a material that originally "
               "used plain Textures");
    m_texture = TextureVariant{&TextureManager::getTexture(filepath)};
  }

  /** @brief Assigns a texture sheet entry. */
  void setTextureSheet(TextureSheet &sheet, unsigned short id)
  {
    P_ASSERT_W(isTextureSheet(),
               "You are defining a Texture in a material that originally "
               "used Texture Sheets");
    m_texture = TextureVariant{SheetStruct{&sheet, id}};
  }
};

} // namespace pain
