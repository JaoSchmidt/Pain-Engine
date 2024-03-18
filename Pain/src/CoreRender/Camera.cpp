#include "CoreRender/Camera.h"
#include "CoreFiles/LogWrapper.h"

#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pain
{

// ======================================================================== //
// OrthographicCamera
// ======================================================================== //

OrthographicCamera::OrthographicCamera(float left, float right, float bottom,
                                       float top)
    : Camera()
{
  m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
  m_ViewMatrix = glm::mat4(1.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::RecalculateViewMatrix(glm::vec3 position,
                                               float rotation)
{
  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 1, 0));

  m_ViewMatrix = glm::inverse(transform);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::SetProjection(float left, float right, float bottom,
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
    : Camera()
{
  m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDegrees),
                                        aspectRatio, 0.1f, 10.0f);
  m_ViewMatrix = glm::mat4(1.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::RecalculateViewMatrix(glm::vec3 position,
                                              glm::vec3 rotation)
{
  glm::mat4 rotationMatrix = glm::rotate(
      glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
  rotationMatrix =
      glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));

  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) * rotationMatrix;

  m_ViewMatrix = glm::inverse(transform);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::SetProjection(float aspectRatio,
                                      float fieldOfViewDegrees)
{

  m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDegrees),
                                        aspectRatio, 0.1f, 10.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

} // namespace pain
