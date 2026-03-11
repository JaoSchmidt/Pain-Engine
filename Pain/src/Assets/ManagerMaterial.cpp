/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** ManagerMaterial.cpp */
#include "Assets/ManagerMaterial.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Buffers/Shader.h"

namespace pain
{

MaterialManager MaterialManager::create()
{
  return MaterialManager( // This assumes the default shaders exist
      {
          *Shader::createFromFile( //
              "Texture", "resources/default/shaders/Texture.glsl"),
          *Shader::createFromFile( //
              "Instancing", "resources/default/shaders/TextureInstancing.glsl"),
          *Shader::createFromFile( //
              "TexturePhong", "resources/default/shaders/TexturePhong.glsl"),
          *Shader::createFromFile( //
              "Circles", "resources/default/shaders/Circles.glsl"),
          *Shader::createFromFile( //
              "Grid", "resources/default/shaders/GridParticles.glsl"),
          *Shader::createFromFile( //
              "Text", "resources/default/shaders/Renderer2dText.glsl"),
          *Shader::createFromFile( //
              "SimpleTriangles", "resources/default/shaders/Triangles.glsl"),
      });
}

MaterialManager::MaterialManager(
    std::array<Shader, static_cast<size_t>(DefaultShader::Count)> shaders)
    : m_defaultShaders(std::move(shaders)),
      m_defaultMaterial{
          Material::create(MaterialCreationInfo{
              .color = Colors::Red,
              .params = ParamSimplest(),
              .shader = m_defaultShaders[0],
          }) //
          ,
      }
{
  int *samplers = new int[backend::getTMU()];
  for (int i = 0; i < backend::getTMUi(); i++)
    samplers[i] = i;

  for (uint8_t i = 0; i < static_cast<uint8_t>(DefaultShader::Count); i++) {
    m_defaultShaders[i].bind();
    if (m_defaultShaders[i].getUniformLocation("u_Textures", false) != -1)
      m_defaultShaders[i].uploadUniformIntArray("u_Textures", samplers,
                                                backend::getTMU());
  }
  delete[] samplers;
};

// ============================================================= //
// **Shader API**
// ============================================================= //

Shader &MaterialManager::getDefaultShader(DefaultShader type)
{
  const size_t index = static_cast<size_t>(type);
  return m_defaultShaders[index];
}

Shader &MaterialManager::loadShaderFromFile(const std::string &name,
                                            const char *filepath)
{
  if (m_shaders.contains(name))
    return m_shaders.at(name);

  auto shaderOpt = Shader::createFromFile(name, filepath);
  if (!shaderOpt) {
    PLOG_W("Failed to load shader {} — falling back to default Texture", name);
    return getDefaultShader(DefaultShader::Texture);
  }

  auto [it, inserted] = m_shaders.emplace(name, std::move(*shaderOpt));

  return it->second;
}

Shader &MaterialManager::loadShaderFromStrings(const std::string &name,
                                               const std::string &vertex,
                                               const std::string &fragment)
{
  if (m_shaders.contains(name))
    return m_shaders.at(name);

  auto shaderOpt = Shader::createFromStrings(name, vertex, fragment);
  if (!shaderOpt) {
    PLOG_W("Failed to create shader {} — falling back to default Texture",
           name);
    return getDefaultShader(DefaultShader::Texture);
  }

  auto [it, inserted] = m_shaders.emplace(name, std::move(*shaderOpt));

  return it->second;
}

Shader &MaterialManager::getShader(const std::string &name)
{
  auto it = m_shaders.find(name);
  if (it == m_shaders.end()) {
    PLOG_W("Shader {} not found — returning default Texture", name);
    return getDefaultShader(DefaultShader::Texture);
  }

  return it->second;
}

// ============================================================= //
// **Material API**
// ============================================================= //

Material &
MaterialManager::createMaterial(const std::string &name,
                                const pain::MaterialCreationInfo &createInfo)
{
  if (m_materials.contains(name))
    return m_materials.at(name);

  Material material = Material::create(createInfo);

  auto [it, inserted] = m_materials.emplace(name, std::move(material));

  return it->second;
}

Material &MaterialManager::getMaterial(const std::string &name)
{
  auto it = m_materials.find(name);
  if (it == m_materials.end()) {
    PLOG_W("Material {} not found, using ", name);
    return m_materials[0]; // Simple phong lighting
  }

  return it->second;
}
Material &MaterialManager::getDefaultMaterial() { return m_defaultMaterial; }
const Material &MaterialManager::getDefaultMaterial() const
{
  return std::as_const(m_defaultMaterial);
}

} // namespace pain
