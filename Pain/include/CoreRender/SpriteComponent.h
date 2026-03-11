
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
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Renderer/Misc.h"
#include "ECS/Components/ComponentManager.h"
#include "Misc/BasicShape.h"
#include "Misc/TextureSheet.h"

#include <variant>

namespace pain
{

/**
 * @brief Configuration used when creating a SpriteComponent.
 *
 * Acts as a lightweight descriptor to initialize a sprite with common visual
 * properties without directly touching the component fields.
 */
struct SpriteCreationInfo {
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

  RenderLayer layer = RenderLayer::Default; /**< Rendering order layer. */
  std::variant<CircleShape, QuadShape, RectShape, TriangleShape> m_shape =
      QuadShape(); /**< Shape geometry. */

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
    return SpriteComponent{.layer = info.layer};
  }
  /** @brief Creates a quad primitive. */
  static SpriteComponent createQuad(const SpriteCreationInfo &info = {},
                                    float size = 0.125f)
  {
    return SpriteComponent{.layer = info.layer, .m_shape = QuadShape(size)};
  }
  /** @brief Creates a rect primitive. */
  static SpriteComponent createRect(const SpriteCreationInfo &info = {},
                                    const glm::vec2 &size = {0.125f, 0.25})
  {
    return SpriteComponent{.layer = info.layer, .m_shape = RectShape(size)};
  }
  /** @brief Creates a circle primitive. */
  static SpriteComponent createCircle(const SpriteCreationInfo &info = {},
                                      float radius = 0.125f)
  {
    return SpriteComponent{.layer = info.layer, .m_shape = CircleShape(radius)};
  }

  /** @brief Creates a colored circle primitive. */
  static SpriteComponent createTriangle(const SpriteCreationInfo &info = {},
                                        float base = 0.125f,
                                        float height = 0.125f)
  {
    return SpriteComponent{.layer = info.layer,
                           .m_shape = TriangleShape(base, height)};
  }
};

} // namespace pain
