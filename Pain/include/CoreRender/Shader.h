#pragma once
#include "pch.h"

#include "Core.h"

namespace pain
{
class  Shader
{
public:
  Shader(const char *filepath);
  Shader(const std::string &name, const char *filepath);
  Shader(const std::string &name, const std::string &vertexShader,
         const std::string &fragmentShader);
  Shader(const std::string &name,
         std::function<std::pair<std::string, std::string>()> fn);

  ~Shader();

  void bind() const;
  void unbind() const;
  static std::pair<std::string, std::string> parseShader(const char *filepath);
  inline const std::string &getName() const { return m_name; };

  // upload variables to shaders
  void uploadUniformInt(const std::string &name, int integer);
  void uploadUniformFloat(const std::string &name, float value);
  void uploadUniformFloat2(const std::string &name, const glm::vec2 &value);
  void uploadUniformFloat3(const std::string &name, const glm::vec3 &value);
  void uploadUniformFloat4(const std::string &name, const glm::vec4 &value);
  void uploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
  void uploadUniformMat4(const std::string &name, const glm::mat4 &matrix);
  void uploadUniformIntArray(const std::string &name, int *values,
                             uint32_t count);

private:
  std::string m_name;
  GLint getUniformLocation(const std::string &name) const;
  uint32_t compileShader(uint32_t type, const std::string &source);
  bool checkLinkProgram(uint32_t programID);
  uint32_t m_programId;
  void createShaderFromStrings(const std::string &vertexShader,
                               const std::string &fragmentShader);
};
} // namespace pain
