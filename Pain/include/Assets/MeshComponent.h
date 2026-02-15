
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file MeshComponent.h
 * @brief Renderable ECS components for textured and primitive sprites.
 *
 * Defines components used by the rendering systems to draw textured sprites,
 * texture-sheet sprites, and simple geometric primitives.
 *
 * These components are data-only and contain no rendering logic. They describe
 * visual state such as size, color, texture source, and render layer.
 */

#pragma once

#include "ECS/Components/ComponentManager.h"

namespace pain
{

enum MeshShape { Cube, Shpere, Count };

/**
 * @brief ECS component representing the geometry of a 3d model.

 * The material need to be added separetly
 */
struct MeshComponent {
  using tag = tag::Mesh;
  /** @brief Variant type representing the sprite texture source. */
  MeshShape shape;
  glm::vec3 size{0.1f}; /**< Mesh Size */
};

} // namespace pain
