#include "Misc/OrthoCameraController.h"

#include "CoreFiles/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"

namespace pain
{

OrthographicCameraController::OrthographicCameraController(float aspectRatio)
    : m_aspectRatio(aspectRatio), m_zoomLevel(1.0f),
      m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel,
               -m_zoomLevel, m_zoomLevel)
{
  m_position = {0.0f, 0.0f, 0.0f};
  m_rotation = 0.0f;
  m_translationSpeed = 1.0f;
  m_rotationSpeed = 180.0f;
  m_zoomSpeed = 0.25f;
}

inline void OrthographicCameraController::setPosition(const glm::vec3 &position)
{
  m_position = position;
  m_camera.RecalculateViewMatrix(m_position, m_rotation);
}

inline void OrthographicCameraController::setRotation(float rotation)
{
  m_rotation = rotation;
  m_camera.RecalculateViewMatrix(m_position, m_rotation);
}

void OrthographicCameraController::onUpdate(double deltaTimeSec)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  glm::vec3 rotVec = {cos(glm::radians(m_rotation)),
                      sin(glm::radians(m_rotation)), 0};
  float moveAmount = (float)(deltaTimeSec * m_translationSpeed * 1.0 +
                             10.0 * state[SDL_SCANCODE_LSHIFT]);

  if (state[SDL_SCANCODE_W])
    m_position -= glm::cross(rotVec, {0.0f, 0.0f, 1.0f}) * moveAmount;
  if (state[SDL_SCANCODE_S])
    m_position += glm::cross(rotVec, {0.0f, 0.0f, 1.0f}) * moveAmount;

  if (state[SDL_SCANCODE_A])
    m_position -= rotVec * moveAmount;
  if (state[SDL_SCANCODE_D])
    m_position += rotVec * moveAmount;

  if (state[SDL_SCANCODE_Q]) {
    m_rotation += m_rotationSpeed * deltaTimeSec;
    setRotation(m_rotation);
  }
  if (state[SDL_SCANCODE_E]) {
    m_rotation -= m_rotationSpeed * deltaTimeSec;
    setRotation(m_rotation);
  }

  setPosition(m_position);

  m_translationSpeed = m_zoomLevel;
}

void OrthographicCameraController::onEvent(const SDL_Event &event)
{
  if (event.type == SDL_MOUSEWHEEL)
    onMouseScrolled(event);
  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      onWindowResized(event);
    }
  }
}

bool OrthographicCameraController::onMouseScrolled(const SDL_Event &event)
{
  m_zoomLevel -= event.wheel.y * 0.25f;
  m_zoomLevel = std::max(m_zoomLevel, 0.25f);
  m_camera.SetProjection(-m_aspectRatio * m_zoomLevel,
                         m_aspectRatio * m_zoomLevel, -m_zoomLevel,
                         m_zoomLevel);
  return false;
}

bool OrthographicCameraController::onWindowResized(const SDL_Event &event)
{
  Renderer2d::setViewport(0, 0, event.window.data1, event.window.data2);
  m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  m_camera.SetProjection(-m_aspectRatio * m_zoomLevel,
                         m_aspectRatio * m_zoomLevel, -m_zoomLevel,
                         m_zoomLevel);
  return false;
}
} // namespace pain
