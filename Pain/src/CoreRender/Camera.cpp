#include "CoreRender/Camera.h"
#include "CoreFiles/LogWrapper.h"

#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pain
{
OrthographicCamera::OrthographicCamera(float left, float right, float bottom,
                                       float top)
{
  m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
  m_ViewMatrix = glm::mat4(1.0f);
}

void OrthographicCamera::RecalculateViewMatrix(glm::vec3 position,
                                               float rotation)
{
  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));

  m_ViewMatrix = glm::inverse(transform);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::SetProjection(float left, float right, float bottom,
                                       float top)
{
  m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
  m_ViewProjectionCacheMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

} // namespace pain
