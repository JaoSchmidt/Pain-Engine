/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Assets/ManagerShader.h"
#include "Core.h"
#include "CoreRender/Buffers/Material.h"

namespace pain
{

/** Holds and owns materials */
class MaterialManager
{
public:
  // ============================================================= //
  // **Creation**
  // ============================================================= //

  static MaterialManager create(ShaderManager &shaderManager);

  // ============================================================= //
  // **Material API**
  // ============================================================= //

  Material &createMaterial(const std::string_view &name,
                           const pain::MaterialCreationInfo &createInfo);

  Material &getMaterial(const std::string_view &name);
  const Material &getMaterial(const std::string_view &name) const;
  /// @brief get simple default material
  Material &getDefaultMaterial();
  const Material &getDefaultMaterial() const;

private:
  MaterialManager(ShaderManager &shaderManager);
  Material m_defaultMaterial;
  std::map<std::string, Material, std::less<>> m_materials = {};
};

} // namespace pain
