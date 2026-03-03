/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file MaterialComponent.h
 * @brief Renderable ECS component for textured and primitive materials.
 */

#pragma once

#include "CoreRender/Buffers/Material.h"
#include "ECS/Components/ComponentManager.h"

namespace pain
{

/**
 * @brief ECS component holding a material.
 */
struct MaterialComponent {
  using tag = tag::Material;
  Material *m_material;
};

} // namespace pain
