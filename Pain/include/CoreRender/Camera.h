#pragma once
#include "pch.h"

#include "Core.h"

namespace pain
{

class Camera
{
public:
  const glm::mat4 &getProjectionMatrix() const { return m_ProjectionMatrix; }
  const glm::mat4 &getViewMatrix() const { return m_ViewMatrix; }
  const glm::mat4 &getViewProjectionMatrix() const
  {
    return m_ViewProjectionCacheMatrix;
  }

protected:
  glm::mat4 m_ProjectionMatrix;
  glm::mat4 m_ViewMatrix;
  glm::mat4 m_ViewProjectionCacheMatrix;
};

class  OrthographicCamera : public Camera
{
public:
  OrthographicCamera(float left, float right, float bottom, float top);
  void RecalculateViewMatrix(const glm::vec3 &m_position,
                             const float m_rotation);
  void SetProjection(float left, float right, float bottom, float top);
};

class  PerspectiveCamera : public Camera
{
public:
  PerspectiveCamera(float aspectRatio, float fieldOfViewDegrees);
  void RecalculateViewMatrix(glm::vec3 m_position, glm::vec3 m_rotation);
  void SetProjection(float aspectRatio, float fieldOfViewDegrees);
};

} // namespace pain
