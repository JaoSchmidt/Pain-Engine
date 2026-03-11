/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Shader.cpp
#include "CoreRender/Buffers/Shader.h"
#include "platform/ShaderBackend.h"

#include "CoreFiles/LogWrapper.h"

namespace pain
{

namespace
{
// Shaders classes don't consume that much memory, they are just a name and a
// number
uint32_t s_binded = 0;
} // namespace

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

int Shader::getUniformLocation(const std::string &name, bool isError) const
{
  P_ASSERT(m_programId == s_binded, "Shader isn't properly binded");
  return backend::getUniformLocation(m_programId, name, isError);
}

void Shader::uploadUniformInt(const std::string &name, int v, bool isError)
{
  backend::uploadUniformInt(getUniformLocation(name, isError), v);
}
void Shader::uploadUniformInt2(const std::string &name, const glm::ivec2 &v,
                               bool isError)
{
  backend::uploadUniformInt2(getUniformLocation(name, isError), v);
}
void Shader::uploadUniformInt3(const std::string &name, const glm::ivec3 &v,
                               bool isError)
{
  backend::uploadUniformInt3(getUniformLocation(name, isError), v);
}
void Shader::uploadUniformInt4(const std::string &name, const glm::ivec4 &v,
                               bool isError)
{
  backend::uploadUniformInt4(getUniformLocation(name, isError), v);
}

void Shader::uploadUniformFloat(const std::string &name, float v, bool isError)
{
  backend::uploadUniformFloat(getUniformLocation(name, isError), v);
}
void Shader::uploadUniformFloat2(const std::string &name, const glm::vec2 &v,
                                 bool isError)
{
  backend::uploadUniformFloat2(getUniformLocation(name, isError), v);
}
void Shader::uploadUniformFloat3(const std::string &name, const glm::vec3 &v,
                                 bool isError)
{
  backend::uploadUniformFloat3(getUniformLocation(name, isError), v);
}
void Shader::uploadUniformFloat4(const std::string &name, const glm::vec4 &v,
                                 bool isError)
{
  backend::uploadUniformFloat4(getUniformLocation(name, isError), v);
}

void Shader::uploadUniformMat3(const std::string &name, const glm::mat3 &m,
                               bool isError)
{
  backend::uploadUniformMat3(getUniformLocation(name, isError), m);
}
void Shader::uploadUniformMat4(const std::string &name, const glm::mat4 &m,
                               bool isError)
{
  backend::uploadUniformMat4(getUniformLocation(name, isError), m);
}

void Shader::uploadUniformIntArray(const std::string &name, int *values,
                                   uint32_t size, bool isError)
{
  backend::uploadUniformIntArray(getUniformLocation(name, isError), values,
                                 size);
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
  auto [vertexShader, fragmentShader] = parseShader(filepath);
  auto shaderOpt =
      createFromStrings(std::filesystem::path(filepath).stem().string(),
                        vertexShader, fragmentShader);
  if (!shaderOpt)
    PLOG_W("Failed to create shader from file {}", filepath);
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
Shader::Shader(Shader &&other) noexcept
    : m_name(std::move(other.m_name)), m_programId(other.m_programId)
{
  other.m_programId = 0;
}
Shader &Shader::operator=(Shader &&other) noexcept
{
  if (this != &other) {
    backend::destroyShaderProgram(m_programId);
    m_name = std::move(other.m_name);
    m_programId = other.m_programId;
    other.m_programId = 0;
  }
  return *this;
}
Shader::~Shader()
{
  if (m_programId)
    backend::destroyShaderProgram(m_programId);
}
} // namespace pain
