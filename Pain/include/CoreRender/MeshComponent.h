/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file MeshComponent.h
 * @brief Renderable ECS components for 3d geometry
 *
 * Defines the component used by the rendering systems to render faces. Basic
 * geometries, like cubes and spheres can be used too
 *
 * These components are data-only and contain no rendering logic. They describe
 * visual state such as size and shape of the object. Your object must contain a
 * 3d Transform to work properly
 */

#pragma once

#include "CoreRender/Renderer/Misc.h"
#include "ECS/Components/ComponentManager.h"

namespace pain
{

/**
 * @brief ECS component representing the geometry of a 3d model.

 * The material need to be added separetly
 */
struct MeshComponent {
  using tag = tag::Mesh;
  /** @brief Variant type representing the sprite texture source. */
  MeshShape shape = Cube;
  float size = 0.125f; /**< Mesh scale */
};

} // namespace pain
