#include "Assets/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "glm/geometric.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

namespace pain
{

PerspectiveCameraController::PerspectiveCameraController(
    float windowWidth, float windowHeight, float fieldOfViewDegrees)
    : m_aspectRatio(windowWidth / windowHeight),
      m_camera(m_aspectRatio, fieldOfViewDegrees),
      m_fieldOfViewDegrees(fieldOfViewDegrees)
{
  m_translationSpeed = 1.0f;
  m_sensitivitySpeed = 0.5f;
  m_zoomSpeed = 10.0f;
  m_windowWidth = windowWidth;
  m_windowHeight = windowHeight;
  Renderer3d::setViewport(0, 0, windowWidth, windowHeight);
  setFrontVector({0.0f, 0.0f, 1.0f});
  setPosition({0.0f, 0.0f, 0.0f});
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void PerspectiveCameraController::onUpdate(double deltaTimeSec)
{
  if (!m_isMovementEnable)
    return;

  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float moveAmount = (float)(deltaTimeSec * m_translationSpeed *
                             (1.0 + 10.0 * state[SDL_SCANCODE_LSHIFT]));
  if (state[SDL_SCANCODE_W])
    m_position += m_cameraFront * moveAmount;
  if (state[SDL_SCANCODE_S])
    m_position -= m_cameraFront * moveAmount;

  if (state[SDL_SCANCODE_A])
    m_position -= glm::cross(m_cameraFront, m_cameraUp) * moveAmount;
  if (state[SDL_SCANCODE_D])
    m_position += glm::cross(m_cameraFront, m_cameraUp) * moveAmount;

  if (state[SDL_SCANCODE_C])
    m_position.y += moveAmount;
  if (state[SDL_SCANCODE_SPACE])
    m_position.y -= moveAmount;

  setPosition(m_position);
}

template <bool IsMoving> void PerspectiveCameraController::setMovementState()
{
  constexpr SDL_bool relativeMouseMode = IsMoving ? SDL_TRUE : SDL_FALSE;
  SDL_SetRelativeMouseMode(relativeMouseMode);
  m_isMovementEnable = IsMoving;
}

void PerspectiveCameraController::onMouseButtonUp(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT) {
    PLOG_I("rotation = ({},{},{})", m_cameraFront.x, m_cameraFront.y,
           m_cameraFront.z);
  }
}

void PerspectiveCameraController::onEvent(const SDL_Event &event)
{
  switch (event.type) {
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE && SDL_GetRelativeMouseMode())
      setMovementState<false>();
    else if (event.key.keysym.sym == SDLK_ESCAPE)
      setMovementState<true>();
    break;
  case SDL_MOUSEBUTTONUP:
    onMouseButtonUp(event);
    break;
  case SDL_MOUSEWHEEL:
    onMouseScrolled(event);
    break;
  case SDL_MOUSEMOTION:
    onMouseMoved(event);
    break;
  case SDL_WINDOWEVENT:
    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
      onWindowResized(event);
    break;
  default:
    break;
  }
}

void PerspectiveCameraController::onMouseMoved(const SDL_Event &e)
{
  if (!m_isMovementEnable)
    return;

  float xoffset = e.motion.xrel * m_sensitivitySpeed;
  float yoffset = e.motion.yrel * m_sensitivitySpeed;

  m_yaw += xoffset;
  m_pitch -= yoffset;

  if (m_pitch > 89.0f)
    m_pitch = 89.0f;
  if (m_pitch < -89.0f)
    m_pitch = -89.0f;

  auto cameraFront = glm::vec3(                              //
      cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), //
      sin(glm::radians(m_pitch)),                            //
      sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))  //
  );
  setFrontVector(cameraFront);
}

void PerspectiveCameraController::onMouseScrolled(const SDL_Event &event)
{
  if (!m_isMovementEnable)
    return;

  m_fieldOfViewDegrees += event.wheel.y * m_zoomSpeed;
  if (m_fieldOfViewDegrees < 1.0f)
    m_fieldOfViewDegrees = 1.0f;
  else if (m_fieldOfViewDegrees > 100.0f)
    m_fieldOfViewDegrees = 100.0f;

  m_camera.SetProjection(m_aspectRatio, m_fieldOfViewDegrees);
}

void PerspectiveCameraController::onWindowResized(const SDL_Event &event)
{
  Renderer3d::setViewport(0, 0, event.window.data1, event.window.data2);
  m_windowWidth = (float)event.window.data1;
  m_windowHeight = (float)event.window.data2;
  m_aspectRatio = m_windowWidth / m_windowHeight;
  m_camera.SetProjection(m_aspectRatio, m_fieldOfViewDegrees);
}
} // namespace pain
