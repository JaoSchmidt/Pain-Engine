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
  static MaterialComponent create(Material &m) { return MaterialComponent(m); }
  static MaterialComponent create(MaterialManager &mm,
                                  const std::string_view &materialName,
                                  const MaterialCreationInfo &info)
  {
    return MaterialComponent(mm.createMaterial(materialName, info));
  }
  const Material *operator->() const { return std::as_const(m_material); };

  const Material &operator*() const { return *m_material; }

  MaterialComponent(MaterialComponent &&o) noexcept
      : m_material(o.m_material) {};
  MaterialComponent &operator=(MaterialComponent &&o) noexcept
  {
    if (this != &o) {
      m_material = o.m_material;
      o.m_material = nullptr;
    }
    return *this;
  }
  MaterialComponent(MaterialComponent &o) : m_material(o.m_material) {}
  MaterialComponent &operator=(const MaterialComponent &o) = delete;
  ~MaterialComponent() = default;

  MaterialComponent(Material &m) : m_material(&m) {};
  const Material *m_material = nullptr;
};

} // namespace pain
