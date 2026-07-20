/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Assets/ManagerMaterial.h"
#include "CoreFiles/LogWrapper.h"

namespace pain
{

MaterialManager MaterialManager::create(ShaderManager &shaderManager)
{
  return MaterialManager(shaderManager);
}

MaterialManager::MaterialManager(ShaderManager &shaderManager)
    : m_defaultMaterial{
          Material::create(MaterialCreationInfo{
              .color = Colors::Red,
              .shader = shaderManager.getDefaultShader(DefaultShader::Texture),
          }) //
      } {};

// ============================================================= //
// **Material API**
// ============================================================= //

Material &
MaterialManager::createMaterial(const std::string_view &name,
                                const pain::MaterialCreationInfo &createInfo)
{
  if (auto it = m_materials.find(name); it != m_materials.end()) {
    PLOG_W(
        "Attention: You are re-creating the material {} which already exists "
        "inside the material manager, perhaps you meant to use "
        "getMaterial(\"{}\")?",
        name, name);
    return it->second;
  }
  Material material = Material::create(createInfo);

  auto [it, inserted] = m_materials.emplace(name, std::move(material));

  return it->second;
}
const Material &MaterialManager::getMaterial(const std::string_view &name) const
{
  auto it = m_materials.find(name);
  if (it == m_materials.end()) {
    PLOG_W("Material {} not found, using ", name);
    return m_defaultMaterial;
  }

  return it->second;
}
Material &MaterialManager::getMaterial(const std::string_view &name)
{
  auto it = m_materials.find(name);
  if (it == m_materials.end()) {
    PLOG_W("Material {} not found, using ", name);
    return m_defaultMaterial;
  }

  return it->second;
}
Material &MaterialManager::getDefaultMaterial() { return m_defaultMaterial; }
const Material &MaterialManager::getDefaultMaterial() const
{
  return std::as_const(m_defaultMaterial);
}

} // namespace pain
