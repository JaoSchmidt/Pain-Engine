#pragma once
#include "pch.gch"

#include "Core.h"

namespace pain
{

class EXPORT OrthographicCamera
{
public:
  OrthographicCamera(float left, float right, float bottom, float top);

  void SetProjection(float left, float right, float bottom, float top);
  const glm::mat4 &getProjectionMatrix() const { return m_ProjectionMatrix; }
  const glm::mat4 &getViewMatrix() const { return m_ViewMatrix; }
  const glm::mat4 &getViewProjectionMatrix() const
  {
    return m_ViewProjectionCacheMatrix;
  }
  void RecalculateViewMatrix(glm::vec3 m_position, float m_rotation);

private:
  glm::mat4 m_ProjectionMatrix;
  glm::mat4 m_ViewMatrix;
  glm::mat4 m_ViewProjectionCacheMatrix;
};

} // namespace pain
