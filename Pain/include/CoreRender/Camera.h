#pragma once
#include "pch.h"

#include "Core.h"

#include <glm/gtc/matrix_transform.hpp>
namespace pain
{

class CameraMatrices
{
public:
  const glm::mat4 &getProjectionMatrix() const { return m_ProjectionMatrix; }
  const glm::mat4 &getViewMatrix() const { return m_ViewMatrix; }
  const glm::mat4 &getViewProjectionMatrix() const
  {
    return m_ViewProjectionCacheMatrix;
  }
  const glm::ivec2 &getResolution() const { return m_resolution; };
  void setResolution(int w, int h) { m_resolution = glm::ivec2(w, h); };

  CameraMatrices() = default;
  CameraMatrices(float left, float right, float bottom, float top, int resWidth,
                 int resHeight)
      : m_ProjectionMatrix{glm::ortho(left, right, bottom, top, -1.0f, 1.0f)},
        m_ViewMatrix{glm::mat4(1.0f)},
        m_ViewProjectionCacheMatrix{m_ProjectionMatrix * m_ViewMatrix},
        m_resolution{resWidth, resHeight} {};

protected:
  glm::mat4 m_ProjectionMatrix;
  glm::mat4 m_ViewMatrix;
  glm::mat4 m_ViewProjectionCacheMatrix;
  glm::ivec2 m_resolution;
};

class OrthographicMatrices : public CameraMatrices
{
public:
  OrthographicMatrices(float left, float right, float bottom, float top,
                       int resWidth, int resHeight);
  void RecalculateViewMatrix(const glm::vec3 &m_position,
                             const float m_rotation);
  void SetProjection(float left, float right, float bottom, float top);
};

class PerspectiveCamera : public CameraMatrices
{
public:
  PerspectiveCamera(float aspectRatio, float fieldOfViewDegrees);
  void RecalculateViewMatrix(glm::vec3 m_position, glm::vec3 m_rotation);
  void SetProjection(float aspectRatio, float fieldOfViewDegrees);
};

} // namespace pain
