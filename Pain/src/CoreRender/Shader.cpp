
#include "Shader.h"
#include "LogWrapper.h"
#include <fstream>
#include <sstream>

#include <GL/glew.h>

std::pair<std::string, std::string> Shader::parseShader(const char *filepath)
{
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
      ss[(int)type] << line << '\n';
    }
  }
  return {ss[0].str(), ss[1].str()};
}

uint32_t Shader::compileShader(uint32_t type, const std::string &source)
{
  uint32_t id = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    PLOG_W("Failed to compile {}",
           type == GL_VERTEX_SHADER ? "vertex" : "fragment");
    PLOG_W("{}", message);
    glDeleteShader(id);
    return 0;
  }
  return id;
};

bool Shader::checkLinkProgram(uint32_t programID)
{
  int linkStatus;
  glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    int infoLenght;
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLenght);
    char *buffer = (char *)alloca(infoLenght * sizeof(char));
    glGetProgramInfoLog(programID, infoLenght, &infoLenght, buffer);
    PLOG_W("Failed to Link Program {}", programID);
    PLOG_W("{}", buffer);
    return false;
  }
  return true;
}

Shader::Shader(std::function<std::pair<std::string, std::string>()> fn)
{
  auto [vertexShader, fragmentShader] = fn();
  createShaderFromStrings(vertexShader, fragmentShader);
}

Shader::Shader(const std::string &vertexShader,
               const std::string &fragmentShader)
{
  createShaderFromStrings(vertexShader, fragmentShader);
}

void Shader::createShaderFromStrings(const std::string &vertexShader,
                                     const std::string &fragmentShader)
{

  uint32_t vs = compileShader(GL_VERTEX_SHADER, vertexShader);
  uint32_t fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
  m_programID = glCreateProgram();
  glAttachShader(m_programID, vs);
  glAttachShader(m_programID, fs);
  glLinkProgram(m_programID);

  if (!checkLinkProgram(m_programID)) {
    glDetachShader(m_programID, vs);
    glDetachShader(m_programID, fs);
    return;
  }
  glValidateProgram(m_programID);

  glDeleteShader(vs);
  glDeleteShader(fs);
}

Shader::~Shader() { glDeleteProgram(m_programID); };

void Shader::bind() const { glUseProgram(m_programID); };
void Shader::unbind() const { glUseProgram(0); };
