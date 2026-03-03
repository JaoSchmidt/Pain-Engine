/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** MaterialManager.h */
#pragma once

#include "Core.h"
#include "CoreRender/Buffers/Material.h"
#include "CoreRender/Buffers/Shader.h"

namespace pain
{
enum class DefaultShader : uint8_t {
  Texture,         // Texture.glsl
  Simple3d,        // TextureInstancing.glsl
  TexturePhong,    // TexturePhong.glsl
  Circles,         // Circles.glsl
  Grid,            // GridParticles.glsl
  Text,            // Renderer2dText.glsl
  SimpleTriangles, // SimpleTriangles.glsl
  Count
};

struct MaterialKey {
  Shader *shader;
  std::variant<ParamPBR, ParamPhong, ParamSimplest> params;
  uint32_t flags; // Transparent, DoubleSided, etc.
  auto operator<=>(const MaterialKey &) const = default;
};

/** Holds and owns shaders and materials */
class MaterialManager
{
public:
  // ============================================================= //
  // **Creation**
  // ============================================================= //

  static MaterialManager create();

  // ============================================================= //
  // **Shader API**
  // ============================================================= //

  Shader &loadShaderFromFile(const std::string &name, const char *filepath);

  Shader &loadShaderFromStrings(const std::string &name,
                                const std::string &vertex,
                                const std::string &fragment);

  Shader &getShader(const std::string &name);
  Shader &getDefaultShader(DefaultShader type);

  // ============================================================= //
  // **Material API**
  // ============================================================= //

  Material &createMaterial(const std::string &name,
                           const pain::MaterialCreationInfo &createInfo);

  Material &getMaterial(const std::string &name);

private:
  MaterialManager(
      std::array<Shader, static_cast<size_t>(DefaultShader::Count)> shaders);
  std::array<Shader, static_cast<size_t>(DefaultShader::Count)>
      m_defaultShaders;
  std::map<std::string, Shader> m_shaders = {};
  std::map<std::string, Material> m_materials = {};
};

} // namespace pain
