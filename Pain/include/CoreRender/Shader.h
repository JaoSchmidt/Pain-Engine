#pragma once
#include "pch.h"

#include "Core.h"

namespace pain
{
class Shader
{
public:
  static std::optional<Shader> createFromFile(const char *filepath);
  static std::optional<Shader> createFromFile(const std::string &name,
                                              const char *filepath);
  static std::optional<Shader> createFromStrings(const std::string &name,
                                                 const std::string &vertex,
                                                 const std::string &fragment);
  static std::optional<Shader>
  createFromFn(const std::string &name,
               std::function<std::pair<std::string, std::string>()> fn);

  ~Shader();
  NONCOPYABLE(Shader);
  Shader(Shader &&o);
  Shader &operator=(Shader &&o);
  const uint32_t getId() const { return m_programId; }

  void bind() const;
  void unbind() const;
  static std::pair<std::string, std::string> parseShader(const char *filepath);
  inline const std::string &getName() const { return m_name; };

  // upload variables to shaders
  void uploadUniformInt(const std::string &name, int integer);
  void uploadUniformInt2(const std::string &name, const glm::ivec2 &value);
  void uploadUniformInt3(const std::string &name, const glm::ivec3 &value);
  void uploadUniformInt4(const std::string &name, const glm::ivec4 &value);
  void uploadUniformFloat(const std::string &name, float value);
  void uploadUniformFloat2(const std::string &name, const glm::vec2 &value);
  void uploadUniformFloat3(const std::string &name, const glm::vec3 &value);
  void uploadUniformFloat4(const std::string &name, const glm::vec4 &value);
  void uploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
  void uploadUniformMat4(const std::string &name, const glm::mat4 &matrix);
  void uploadUniformIntArray(const std::string &name, int *values,
                             uint32_t count);

private:
  Shader() = default;
  std::string m_name = "undefined";
  GLint getUniformLocation(const std::string &name) const;
  uint32_t compileShader(uint32_t type, const std::string &source);
  bool checkLinkProgram(uint32_t programID);
  uint32_t m_programId = 0;
  void createShaderFromStrings(const std::string &vertexShader,
                               const std::string &fragmentShader);
};
} // namespace pain
