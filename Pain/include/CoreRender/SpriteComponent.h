
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

#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Shapes.h"
#include "ECS/Components/ComponentManager.h"

#include <variant>

namespace pain
{

/**
 * @brief Descriptor used to construct a SpriteComponent.
 *
 * Encapsulates all parameters required to initialize a sprite. It is intended
 * to be passed to SpriteComponent::create() and allows callers to configure
 * sprites without directly accessing component fields.
 *
 *  - shape defines the geometric representation of the sprite. The default
 * is a QuadShape. Some shapes like circles need to be drawn using materials
 *  - layer defines the order in which the sprite will be draw
 */
struct SpriteCreationInfo {
  RenderLayer layer = RenderLayer::D; /**< Rendering order layer. */
  ShapeVariant shape = QuadShape{};
};

/**
 * @brief ECS component representing a textured sprite.
 *
 * Stores the minimal data required by rendering systems to draw a sprite.
 * This includes:
 *  - render ordering (layer)
 *  - geometric shape information.
 *
 * The component is intentionally data-only and contains no rendering logic.
 * All rendering behavior is handled by external systems.
 *
 * Shape data is stored internally as a variant
 */
struct SpriteComponent {
  using tag = tag::Sprite;
  static_assert(std::is_copy_constructible_v<ShapeVariant>);
  static_assert(std::is_copy_assignable_v<ShapeVariant>);
  RenderLayer layer = RenderLayer::D; /**< Rendering order layer. */
  ShapeVariant m_shape = RectShape{}; /**< Shape geometry. */

  // ------------------------------------------------------------
  // Factory functions
  // ------------------------------------------------------------
  /**
   * @brief Creates a sprite
   * @param info Sprite configuration parameters.
   */
  static SpriteComponent create(const SpriteCreationInfo &info = {})
  {
    SpriteComponent s;
    s.layer = info.layer;
    s.m_shape = info.shape;
    return s;
  }
};

} // namespace pain
