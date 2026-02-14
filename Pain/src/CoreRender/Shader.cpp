/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Shader.cpp
#include "CoreRender/Shader.h"
#include "platform/ShaderBackend.h"

#include "CoreFiles/LogWrapper.h"

namespace pain
{

namespace
{
// Shaders classes don't consume that much memory, they are just a name and a
// number
std::map<std::string, Shader> s_shaders;
uint32_t s_binded = 0;
} // namespace

Shader::~Shader()
{
  if (m_programId)
    backend::destroyShaderProgram(m_programId);
}

Shader::Shader(Shader &&o)
    : m_name(std::move(o.m_name)), m_programId(o.m_programId)
{
  o.m_programId = 0;
}

Shader &Shader::operator=(Shader &&o)
{
  if (this != &o) {
    m_name = std::move(o.m_name);
    m_programId = o.m_programId;
    o.m_programId = 0;
  }
  return *this;
}

void Shader::bind() const
{
  backend::bindShader(m_programId);
#ifndef NDEBUG
  s_binded = m_programId;
#endif
}

void Shader::unbind() const
{
  backend::unbindShader();
#ifndef NDEBUG
  s_binded = 0;
#endif
}

// ----------------------------------------------------------
// Uniform uploads
// ----------------------------------------------------------

int Shader::getUniformLocation(const std::string &name) const
{
  P_ASSERT(m_programId == s_binded, "Shader isn't properly binded");
  return backend::getUniformLocation(m_programId, name);
}

void Shader::uploadUniformInt(const std::string &name, int v)
{
  backend::uploadUniformInt(getUniformLocation(name), v);
}
void Shader::uploadUniformInt2(const std::string &name, const glm::ivec2 &v)
{
  backend::uploadUniformInt2(getUniformLocation(name), v);
}
void Shader::uploadUniformInt3(const std::string &name, const glm::ivec3 &v)
{
  backend::uploadUniformInt3(getUniformLocation(name), v);
}
void Shader::uploadUniformInt4(const std::string &name, const glm::ivec4 &v)
{
  backend::uploadUniformInt4(getUniformLocation(name), v);
}

void Shader::uploadUniformFloat(const std::string &name, float v)
{
  backend::uploadUniformFloat(getUniformLocation(name), v);
}
void Shader::uploadUniformFloat2(const std::string &name, const glm::vec2 &v)
{
  backend::uploadUniformFloat2(getUniformLocation(name), v);
}
void Shader::uploadUniformFloat3(const std::string &name, const glm::vec3 &v)
{
  backend::uploadUniformFloat3(getUniformLocation(name), v);
}
void Shader::uploadUniformFloat4(const std::string &name, const glm::vec4 &v)
{
  backend::uploadUniformFloat4(getUniformLocation(name), v);
}

void Shader::uploadUniformMat3(const std::string &name, const glm::mat3 &m)
{
  backend::uploadUniformMat3(getUniformLocation(name), m);
}
void Shader::uploadUniformMat4(const std::string &name, const glm::mat4 &m)
{
  backend::uploadUniformMat4(getUniformLocation(name), m);
}

void Shader::uploadUniformIntArray(const std::string &name, int *values,
                                   uint32_t count)
{
  backend::uploadUniformIntArray(getUniformLocation(name), values, count);
}

// ----------------------------------------------------------
// Creation
// ----------------------------------------------------------

std::optional<Shader> Shader::createFromStrings(const std::string &name,
                                                const std::string &vertex,
                                                const std::string &fragment)
{
  Shader shader;
  shader.m_name = name;
  shader.m_programId = backend::createShaderProgram(name, vertex, fragment);

  if (!shader.m_programId)
    return std::nullopt;

  return shader;
}

std::pair<std::string, std::string> Shader::parseShader(const char *filepath)
{
  P_ASSERT_W(std::filesystem::exists(std::filesystem::absolute(filepath)),
             "Absolute file {} does not exist.",
             std::filesystem::absolute(filepath).string());
  P_ASSERT(std::filesystem::exists(filepath), "File {} does not exist.",
           filepath);

  enum class ShadertType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
  std::ifstream stream(filepath);
  std::string line;
  std::stringstream ss[2];
  ShadertType type = ShadertType::NONE;

  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        type = ShadertType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) {
        type = ShadertType::FRAGMENT;
      }
    } else {
      P_ASSERT((int)type != -1,
               "Could not identify shader type, make sure it has "
               "\"#shader vertex\" or \"#shader fragment\" in the .glsl")
      ss[(int)type] << line << '\n';
    }
  }
  return {ss[0].str(), ss[1].str()};
}

std::optional<Shader> Shader::createFromFile(const char *filepath)
{
  if (!std::filesystem::exists(filepath)) {
    PLOG_W("Shader file {} does not exist.", filepath);
    return std::nullopt;
  }
  auto it = s_shaders.find(filepath);
  if (it != s_shaders.end())
    return Shader{it->second.getName(), it->second.getId()};

  auto [vertexShader, fragmentShader] = parseShader(filepath);
  auto shaderOpt =
      createFromStrings(std::filesystem::path(filepath).stem().string(),
                        vertexShader, fragmentShader);
  if (!shaderOpt)
    PLOG_W("Failed to create shader from file {}", filepath);
  else
    s_shaders.insert(
        std::make_pair(std::string(filepath),
                       Shader{shaderOpt->getName(), shaderOpt->getId()}));
  return shaderOpt;
}

std::optional<Shader> Shader::createFromFile(const std::string &name,
                                             const char *filepath)
{
  if (!std::filesystem::exists(filepath)) {
    PLOG_W("Shader file {} does not exist.", filepath);
    return std::nullopt;
  }

  auto [vertexShader, fragmentShader] = parseShader(filepath);
  return createFromStrings(name, vertexShader, fragmentShader);
}

std::optional<Shader>
Shader::createFromFn(const std::string &name,
                     std::function<std::pair<std::string, std::string>()> fn)
{
  auto [vertex, fragment] = fn();
  return createFromStrings(name, vertex, fragment);
}

Shader::Shader(std::string name, uint32_t programId)
    : m_name(name), m_programId(programId) {};

} // namespace pain
