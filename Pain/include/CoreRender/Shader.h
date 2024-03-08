#pragma once

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
  unsigned int compileShader(unsigned int type, const std::string &source);
  bool checkLinkProgram(unsigned int programID);
  unsigned int m_programID;
  void createShaderFromStrings(const std::string &vertexShader,
                               const std::string &fragmentShader);
};
