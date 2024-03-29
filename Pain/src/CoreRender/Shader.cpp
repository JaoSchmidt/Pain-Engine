
#include "CoreRender/Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core.h"
#include "CoreFiles/LogWrapper.h"

namespace pain
{
Shader::~Shader() { glDeleteProgram(m_programId); };

void Shader::bind() const { glUseProgram(m_programId); };
void Shader::unbind() const { glUseProgram(0); };

// ========================================================== //
// Upload stuff to Shader
// ========================================================== //

GLint Shader::getUniformLocation(const std::string &name) const
{
  GLint location = glGetUniformLocation(m_programId, name.c_str());
  P_ASSERT_W(location != -1, "Uniform {} was not found on shader program {}",
             name, std::to_string(m_programId));
  return location;
}
void Shader::uploadUniformInt(const std::string &name, int value)
{
  GLint location = getUniformLocation(name);
  glUniform1i(location, value);
}
void Shader::uploadUniformFloat(const std::string &name, float value)
{
  GLint location = getUniformLocation(name);
  glUniform1f(location, value);
}
void Shader::uploadUniformFloat2(const std::string &name, const glm::vec2 &val)
{
  GLint location = getUniformLocation(name);
  glUniform2f(location, val.x, val.y);
}
void Shader::uploadUniformFloat3(const std::string &name, const glm::vec3 &val)
{
  GLint location = getUniformLocation(name);
  glUniform3f(location, val.x, val.y, val.z);
}
void Shader::uploadUniformFloat4(const std::string &name, const glm::vec4 &val)
{
  GLint location = getUniformLocation(name);
  glUniform4f(location, val.x, val.y, val.z, val.w);
}
void Shader::uploadUniformMat3(const std::string &name, const glm::mat3 &matrix)
{
  GLint location = getUniformLocation(name);
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::uploadUniformMat4(const std::string &name, const glm::mat4 &matrix)
{
  GLint location = getUniformLocation(name);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

// ========================================================== //
// Shader interpretation and link
// ========================================================== //

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

// ========================================================== //
// Shader creation
// ========================================================== //

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
      P_ASSERT((int)type != -1,
               "Could not identify shader type, make sure it has "
               "\"#shader vertex\" or \"#shader fragment\" in the .glsl")
      ss[(int)type] << line << '\n';
    }
  }
  return {ss[0].str(), ss[1].str()};
}

Shader::Shader(const char *filepath)
{
  // Extract name from filepath
  std::filesystem::path fp(filepath);
  m_name = fp.filename().stem().string();

  auto [vertexShader, fragmentShader] = parseShader(filepath);
  createShaderFromStrings(vertexShader, fragmentShader);
}

Shader::Shader(const std::string &name, const char *filepath)
{
  auto [vertexShader, fragmentShader] = parseShader(filepath);
  m_name = name;
  createShaderFromStrings(vertexShader, fragmentShader);
}

Shader::Shader(const std::string &name, const std::string &vertexShader,
               const std::string &fragmentShader)
{
  createShaderFromStrings(vertexShader, fragmentShader);
}

Shader::Shader(const std::string &name,
               std::function<std::pair<std::string, std::string>()> fn)
{
  auto [vertexShader, fragmentShader] = fn();
  m_name = name;
  createShaderFromStrings(vertexShader, fragmentShader);
}

void Shader::createShaderFromStrings(const std::string &vertexShader,
                                     const std::string &fragmentShader)
{

  uint32_t vs = compileShader(GL_VERTEX_SHADER, vertexShader);
  uint32_t fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
  m_programId = glCreateProgram();
  glAttachShader(m_programId, vs);
  glAttachShader(m_programId, fs);
  glLinkProgram(m_programId);

  if (!checkLinkProgram(m_programId)) {
    glDetachShader(m_programId, vs);
    glDetachShader(m_programId, fs);
    return;
  }
  glValidateProgram(m_programId);

  glDeleteShader(vs);
  glDeleteShader(fs);
}

} // namespace pain
