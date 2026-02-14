/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// ShaderBackend.cpp
#include "ShaderBackend.h"

#ifdef PAIN_RENDERER_OPENGL

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include "CoreFiles/LogWrapper.h"
#include "OpenGLDebugger.h"

namespace pain::backend
{

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static uint32_t compileShader(uint32_t type, const std::string &source)
{
  uint32_t id = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  P_OPENGL_CHECK("Failed to compile shader");

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length);
    glGetShaderInfoLog(id, length, &length, message);
    PLOG_W("Shader compilation failed:\n{}", message);
    glDeleteShader(id);
    return 0;
  }
  return id;
}

static bool linkProgram(uint32_t program)
{
  int status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    char *buffer = (char *)alloca(length);
    glGetProgramInfoLog(program, length, &length, buffer);
    PLOG_W("Shader program link failed:\n{}", buffer);
    return false;
  }
  return true;
}

// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------

uint32_t createShaderProgram(const std::string &name,
                             const std::string &vertexSrc,
                             const std::string &fragmentSrc)
{
  uint32_t vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
  uint32_t fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
  if (!vs || !fs)
    return 0;

  uint32_t program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  if (!linkProgram(program)) {
    glDeleteProgram(program);
    program = 0;
  }

  glDetachShader(program, vs);
  glDetachShader(program, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);

  PLOG_I("Shader program {} created ({})", program, name);
  return program;
}

void destroyShaderProgram(uint32_t programId) { glDeleteProgram(programId); }

void bindShader(uint32_t programId)
{
  glUseProgram(programId);
  P_OPENGL_CHECK("Failed to bind shader {}", programId);
}

void unbindShader() { glUseProgram(0); }

// ------------------------------------------------------------
// Uniforms
// ------------------------------------------------------------

int getUniformLocation(uint32_t programId, const std::string &name)
{
  int loc = glGetUniformLocation(programId, name.c_str());
  P_ASSERT_W(loc != -1, "Uniform {} not found on program {}", name, programId);
  return loc;
}

void uploadUniformInt(int loc, int v) { GL_CALL(glUniform1i(loc, v)); }
void uploadUniformInt2(int loc, const glm::ivec2 &v)
{
  GL_CALL(glUniform2i(loc, TP_VEC2(v)));
}
void uploadUniformInt3(int loc, const glm::ivec3 &v)
{
  GL_CALL(glUniform3i(loc, TP_VEC3(v)));
}
void uploadUniformInt4(int loc, const glm::ivec4 &v)
{
  GL_CALL(glUniform4i(loc, TP_VEC4(v)));
}
void uploadUniformFloat(int loc, float v) { GL_CALL(glUniform1f(loc, v)); }
void uploadUniformFloat2(int loc, const glm::vec2 &v)
{
  GL_CALL(glUniform2f(loc, TP_VEC2(v)));
}
void uploadUniformFloat3(int loc, const glm::vec3 &v)
{
  GL_CALL(glUniform3f(loc, TP_VEC3(v)));
}
void uploadUniformFloat4(int loc, const glm::vec4 &v)
{
  GL_CALL(glUniform4f(loc, TP_VEC4(v)));
}
void uploadUniformMat3(int loc, const glm::mat3 &m)
{
  GL_CALL(glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(m)));
}
void uploadUniformMat4(int loc, const glm::mat4 &m)
{
#ifndef NDEBUG
  GLint currentProgram = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

  if (currentProgram == 0) {
    PLOG_E("glUniformMatrix4fv called with no program bound");
    DEBUG_BREAK();
    return;
  }

  if (loc == -1) {
    PLOG_E("glUniformMatrix4fv called with loc = -1");
    DEBUG_BREAK();
    return;
  }
#endif
  GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m)));
}
void uploadUniformIntArray(int loc, int *values, uint32_t count)
{
  GL_CALL(glUniform1iv(loc, static_cast<int32_t>(count), values));
}

} // namespace pain::backend

#endif
