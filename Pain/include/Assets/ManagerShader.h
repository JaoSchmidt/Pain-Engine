/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"
#include "CoreRender/Buffers/Shader.h"

namespace pain
{
enum class DefaultShader : uint8_t {
  Texture = 0,     // Texture.glsl
  Instancing,      // TextureInstancing.glsl
  TexturePhong,    // TexturePhong.glsl
  Circles,         // Circles.glsl
  Grid,            // GridParticles.glsl
  Text,            // Renderer2dText.glsl
  SimpleTriangles, // SimpleTriangles.glsl
  Count
};

class ShaderManager
{
public:
  static ShaderManager create();

  Shader &loadShaderFromFile(const std::string &name, const char *filepath);

  Shader &loadShaderFromStrings(const std::string &name,
                                const std::string &vertex,
                                const std::string &fragment);

  Shader &getShader(const std::string &name);
  Shader &getDefaultShader(DefaultShader type);

private:
  ShaderManager(
      std::array<Shader, static_cast<size_t>(DefaultShader::Count)> shaders);
  std::array<Shader, static_cast<size_t>(DefaultShader::Count)>
      m_defaultShaders;
  std::map<std::string, Shader> m_shaders = {};
};

} // namespace pain
