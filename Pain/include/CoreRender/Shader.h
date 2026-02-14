/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** Shader.h */
#pragma once
#include "Core.h"
#include "pch.h"

#include <functional>
#include <glm/glm.hpp>
#include <optional>
#include <string>

namespace pain
{

/** GPU shader program abstraction with API-agnostic uniform uploads. */
class Shader
{
public:
  // ============================================================= //
  // **Creation**
  // ============================================================= //

  /** Creates a shader by parsing a combined shader file. */
  static std::optional<Shader> createFromFile(const char *filepath);

  /** Creates a shader with an explicit debug name from a combined shader file.
   */
  static std::optional<Shader> createFromFile(const std::string &name,
                                              const char *filepath);

  /** Creates a shader directly from vertex and fragment source strings. */
  static std::optional<Shader> createFromStrings(const std::string &name,
                                                 const std::string &vertex,
                                                 const std::string &fragment);

  /** Creates a shader from a generator function returning vertex and fragment
   * sources. */
  static std::optional<Shader>
  createFromFn(const std::string &name,
               std::function<std::pair<std::string, std::string>()> fn);

  ~Shader();
  Shader(Shader &&o);
  Shader &operator=(Shader &&o);

  /** Returns the backend shader program handle. */
  uint32_t getId() const { return m_programId; }

  /** Binds the shader for rendering. */
  void bind() const;

  /** Unbinds the currently active shader. */
  void unbind() const;

  /** Returns the shader debug name. */
  inline const std::string &getName() const { return m_name; }

  // ============================================================= //
  // **Uniform uploads**
  // ============================================================= //

  void uploadUniformInt(const std::string &name, int value);
  void uploadUniformInt2(const std::string &name, const glm::ivec2 &value);
  void uploadUniformInt3(const std::string &name, const glm::ivec3 &value);
  void uploadUniformInt4(const std::string &name, const glm::ivec4 &value);
  void uploadUniformFloat(const std::string &name, float value);
  void uploadUniformFloat2(const std::string &name, const glm::vec2 &value);
  void uploadUniformFloat3(const std::string &name, const glm::vec3 &value);
  void uploadUniformFloat4(const std::string &name, const glm::vec4 &value);
  void uploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
  void uploadUniformMat4(const std::string &name, const glm::mat4 &matrix);
  /** Uploads an array of int uniforms. */
  void uploadUniformIntArray(const std::string &name, int *values,
                             uint32_t count);

  // ============================================================= //
  // **Utilities**
  // ============================================================= //

  /** Parses a combined shader file into vertex and fragment source strings. */
  static std::pair<std::string, std::string> parseShader(const char *filepath);

private:
  Shader() = default;
  Shader(std::string name, uint32_t programId);
  int getUniformLocation(const std::string &name) const;
  std::string m_name = "undefined";
  uint32_t m_programId = 0;
};

} // namespace pain
