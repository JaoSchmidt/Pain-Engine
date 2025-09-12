#include "CoreRender/Camera.h"
#include "CoreFiles/LogWrapper.h"
#include "glm/ext/matrix_transform.hpp"

#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pain
{

// ======================================================================== //
// OrthographicCamera
// ======================================================================== //

OrthographicMatrices::OrthographicMatrices(float left, float right,
                                           float bottom, float top)
    : CameraMatrices()
{
  m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
  m_ViewMatrix = glm::mat4(1.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicMatrices::RecalculateViewMatrix(const glm::vec3 &position,
                                                 const float rotation)
{
  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));

  m_ViewMatrix = glm::inverse(transform);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicMatrices::SetProjection(float left, float right, float bottom,
                                         float top)
{
  m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

// ======================================================================== //
// PerspectiveCamera
// ======================================================================== //

PerspectiveCamera::PerspectiveCamera(float aspectRatio,
                                     float fieldOfViewDegrees)
    : CameraMatrices()
{
  m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDegrees),
                                        aspectRatio, 0.1f, 10.0f);
  m_ViewMatrix = glm::mat4(1.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::RecalculateViewMatrix(glm::vec3 position,
                                              glm::vec3 frontCamera)
{
  glm::vec3 frontUnit = glm::normalize(frontCamera);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  // m_ViewMatrix = glm::inverse(transform);
  m_ViewMatrix = glm::lookAt(position, position + frontUnit, up);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::SetProjection(float aspectRatio,
                                      float fieldOfViewDegrees)
{

  m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDegrees),
                                        aspectRatio, 0.1f, 100.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

} // namespace pain
