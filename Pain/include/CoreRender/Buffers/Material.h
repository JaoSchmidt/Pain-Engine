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
#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "platform/ContextBackend.h"

#include <variant>

namespace pain
{
struct ParamPBR {         /// Physically Based Rendering
  float roughness = 1.0f; ///< 1 = matte
  float metallic = 0.0f;  ///< 0 = dielectric
  float emission = 0.0f;  ///< Light emission
  auto operator<=>(const ParamPBR &) const = default;
};
struct ParamPhong {       // Phong only parameters
  float ambient = 1.0f;   ///< 1 = full lit
  float highlight = 0.2f; ///< 1 = light source dominates
  float diffuse = 0.3f;   ///< 1 = Shadow
  auto operator<=>(const ParamPhong &) const = default;
};
struct ParamSimplest { // Super simple like Texture.glsl
  int id = 0;
  auto operator<=>(const ParamSimplest &) const = default;
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
  std::variant<ParamPBR, ParamPhong, ParamSimplest> params = ParamPBR{};
  Shader &shader;
  Texture &texture =
      TextureManager::getDefaultTexture(TextureManager::DefaultTexture::Blank);
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
  std::variant<ParamPBR, ParamPhong, ParamSimplest> m_params =
      ParamPBR{};                       ///< Shader specific parameters
  Color m_color = {255, 255, 255, 255}; ///< Tint color.
  float m_tilingFactor = 1.f;           ///< Texture tiling multiplier.
  Shader *m_shader = nullptr;           ///< Instanciated Shader

  uint32_t m_flags = 0; ///< FlagOptions
  enum FlagOptions {
    Unlit = 1 << 0,       //
    Transparent = 1 << 1, //
    DoubleSided = 1 << 2  //
  };

  /** @brief Texture source used by the material.
   * Defaults to a fallback texture to ensure safe rendering, but you should
   * define manually. Otherwise will generate an warning */
  Texture *m_texture =
      &TextureManager::getDefaultTexture(TextureManager::DefaultTexture::Blank);

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
        .m_params = info.params,
        .m_color = info.color,
        .m_tilingFactor = info.tilingFactor,
        .m_shader = &info.shader,
        .m_texture = &info.texture,
    };
  }
};

} // namespace pain
