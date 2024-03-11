#pragma once
#include "pch.gch"

#include "CoreRender/AllBuffers.h"
#include "CoreRender/BufferLayout.h"
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"

namespace pain
{
class SimpleShape
{
public:
  // clang-format off
  inline std::shared_ptr<VertexArray> getVertexArr() 
    const { return m_vertexArray; }
  inline std::shared_ptr<Shader> getShader() const { return m_shader; }
  inline float getPosX() const { return m_position.x; }
  inline float getPosY() const { return m_position.y; }
  inline glm::vec3 getPos() const { return m_position; }
  inline void mvPositiveY(const double dtSeconds) { 
    m_position.y += m_speed.y * dtSeconds; }
  inline void mvPositiveX(const double dtSeconds) {
    m_position.x += m_speed.x * dtSeconds; }
  inline void mvNegativeY(const double dtSeconds) {
    m_position.y -= m_speed.y * dtSeconds; }
  inline void mvNegativeX(const double dtSeconds) {
    m_position.x -= m_speed.x * dtSeconds; }
  // clang-format on
protected:
  glm::vec3 m_speed = glm::vec3(1.0f);
  glm::vec3 m_position = glm::vec3(0.0f);
  std::shared_ptr<VertexArray> m_vertexArray;
  std::shared_ptr<Shader> m_shader;
};

class Rectangle : public SimpleShape
{
public:
  Rectangle(float x, float y, float w, float h);
  Rectangle();
};

class IsocelesTriangle : public SimpleShape
{
public:
  IsocelesTriangle(float w, float h);
};

} // namespace pain
