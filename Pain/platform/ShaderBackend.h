// ShaderBackend.h
#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <string>

namespace pain::backend
{

uint32_t createShaderProgram(const std::string &name,
                             const std::string &vertexSrc,
                             const std::string &fragmentSrc);

void destroyShaderProgram(uint32_t programId);

void bindShader(uint32_t programId);
void unbindShader();

int getUniformLocation(uint32_t programId, const std::string &name);

void uploadUniformInt(int location, int value);
void uploadUniformInt2(int location, const glm::ivec2 &v);
void uploadUniformInt3(int location, const glm::ivec3 &v);
void uploadUniformInt4(int location, const glm::ivec4 &v);
void uploadUniformFloat(int location, float value);
void uploadUniformFloat2(int location, const glm::vec2 &v);
void uploadUniformFloat3(int location, const glm::vec3 &v);
void uploadUniformFloat4(int location, const glm::vec4 &v);
void uploadUniformMat3(int location, const glm::mat3 &m);
void uploadUniformMat4(int location, const glm::mat4 &m);
void uploadUniformIntArray(int location, int *values, uint32_t count);

} // namespace pain::backend
