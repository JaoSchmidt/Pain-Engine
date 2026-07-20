/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Assets/ManagerShader.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Buffers/Shader.h"
#include "platform/ContextBackend.h"

namespace pain
{

ShaderManager ShaderManager::create()
{
  return ShaderManager( // This assumes the default shaders exist
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

ShaderManager::ShaderManager(
    std::array<Shader, static_cast<size_t>(DefaultShader::Count)> shaders)
    : m_defaultShaders(std::move(shaders))
{
  auto samplers = std::make_unique<int[]>(backend::getTMU());
  for (int i = 0; i < backend::getTMUi(); ++i)
    samplers[static_cast<size_t>(i)] = i;
  for (uint8_t i = 0; i < static_cast<uint8_t>(DefaultShader::Count); ++i) {
    m_defaultShaders[i].bind();
    if (m_defaultShaders[i].getUniformLocation("u_Textures", false) != -1)
      m_defaultShaders[i].uploadUniformIntArray("u_Textures", samplers.get(),
                                                backend::getTMU());
  }
};

// ============================================================= //
// **Shader API**
// ============================================================= //

Shader &ShaderManager::getDefaultShader(DefaultShader type)
{
  const size_t index = static_cast<size_t>(type);
  return m_defaultShaders[index];
}

Shader &ShaderManager::loadShaderFromFile(const std::string &name,
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

Shader &ShaderManager::loadShaderFromStrings(const std::string &name,
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

Shader &ShaderManager::getShader(const std::string &name)
{
  auto it = m_shaders.find(name);
  if (it == m_shaders.end()) {
    PLOG_W("Shader {} not found — returning default Texture", name);
    return getDefaultShader(DefaultShader::Texture);
  }

  return it->second;
}

} // namespace pain
