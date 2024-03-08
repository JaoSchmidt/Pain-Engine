#pragma once

#include <cstdint>
#include <functional>
#include <string>

class Shader
{
public:
  Shader(const std::string &vertexShader, const std::string &fragmentShader);
  Shader(std::function<std::pair<std::string, std::string>()> fn);
  ~Shader();

  void bind() const;
  void unbind() const;
  static std::pair<std::string, std::string> parseShader(const char *filepath);

private:
  uint32_t compileShader(uint32_t type, const std::string &source);
  bool checkLinkProgram(uint32_t programID);
  uint32_t m_programID;
  void createShaderFromStrings(const std::string &vertexShader,
                               const std::string &fragmentShader);
};
