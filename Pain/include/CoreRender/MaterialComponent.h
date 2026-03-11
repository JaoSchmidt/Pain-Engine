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

#include "Assets/ManagerMaterial.h"
#include "CoreRender/Buffers/Material.h"
#include "ECS/Components/ComponentManager.h"

namespace pain
{

/**
 * @brief ECS component holding a material.
 */
struct MaterialComponent {
  using tag = tag::Material;
  static MaterialComponent create(MaterialManager &mm)
  {
    return MaterialComponent(mm);
  }
  static MaterialComponent create(Material &m) { return MaterialComponent(m); }
  Material *operator->() { return m_material; };
  const Material *operator->() const { return std::as_const(m_material); };

  Material &operator*() { return *m_material; }
  const Material &operator*() const { return *m_material; }

private:
  MaterialComponent(Material &m) : m_material(&m) {};
  MaterialComponent(MaterialManager &mm)
      : m_material(&mm.getDefaultMaterial()) {};
  Material *m_material;
};

} // namespace pain
