#pragma once
#include "pch.gch"

#include "Core.h"

namespace pain
{
class EXPORT Shader
{
public:
  Shader(const std::string &vertexShader, const std::string &fragmentShader);
  Shader(std::function<std::pair<std::string, std::string>()> fn);
  ~Shader();

  void bind() const;
  void unbind() const;
  static std::pair<std::string, std::string> parseShader(const char *filepath);
  void uploadUniformMat4(const std::string &name, const glm::mat4 &matrix);
  void uploadUniformFloat3(const std::string &name, const glm::vec3 &vec);

private:
  GLint getUniformLocation(const std::string &name) const;
  uint32_t compileShader(uint32_t type, const std::string &source);
  bool checkLinkProgram(uint32_t programID);
  uint32_t m_programId;
  void createShaderFromStrings(const std::string &vertexShader,
                               const std::string &fragmentShader);
};
} // namespace pain
