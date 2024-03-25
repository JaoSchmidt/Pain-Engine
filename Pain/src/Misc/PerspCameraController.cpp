#include "CoreFiles/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

namespace pain
{

PerspectiveCameraController::PerspectiveCameraController(
    float windowWidth, float windowHeight, float fieldOfViewDegrees)
    : m_aspectRatio(windowWidth / windowHeight), m_zoomLevel(1.0f),
      m_camera(m_aspectRatio, fieldOfViewDegrees),
      m_fieldOfViewDegrees(fieldOfViewDegrees)
{
  m_position = {0.0f, 0.0f, 0.0f};
  m_rotation = {0.0f, 0.0f, 0.0f};
  m_translationSpeed = 1.0f;
  m_rotationSpeed = 180.0f;
  m_zoomSpeed = 0.25f;
  Renderer3d::setViewport(0, 0, windowWidth, windowHeight);
}

void PerspectiveCameraController::onUpdate(double deltaTimeSec)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);

  if (state[SDL_SCANCODE_A])
    m_position.x -= m_translationSpeed * deltaTimeSec;
  else if (state[SDL_SCANCODE_D])
    m_position.x += m_translationSpeed * deltaTimeSec;

  if (state[SDL_SCANCODE_W])
    m_position.z -= m_translationSpeed * deltaTimeSec;
  else if (state[SDL_SCANCODE_S])
    m_position.z += m_translationSpeed * deltaTimeSec;

  if (state[SDL_SCANCODE_SPACE])
    m_position.y -= m_translationSpeed * deltaTimeSec;
  else if (state[SDL_SCANCODE_C])
    m_position.y += m_translationSpeed * deltaTimeSec;

  if (state[SDL_SCANCODE_Q]) {
    m_rotation.y += m_rotationSpeed * deltaTimeSec;
    setRotation(m_rotation);
  } else if (state[SDL_SCANCODE_E]) {
    m_rotation.y -= m_rotationSpeed * deltaTimeSec;
    setRotation(m_rotation);
  }

  setPosition(m_position);

  // m_translationSpeed = m_zoomLevel;
}

void PerspectiveCameraController::onEvent(const SDL_Event &event)
{
  // if (event.type == SDL_MOUSEWHEEL)
  //   onMouseScrolled(event);
  // if (event.type == SDL_WINDOWEVENT) {
  //   if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
  //     onWindowResized(event);
  //   }
  // }
}

bool PerspectiveCameraController::onMouseScrolled(const SDL_Event &event)
{
  m_zoomLevel -= event.wheel.y * 0.25f;
  m_zoomLevel = std::max(m_zoomLevel, 0.25f);
  m_camera.SetProjection(m_aspectRatio, m_fieldOfViewDegrees);
  return false;
}

bool PerspectiveCameraController::onWindowResized(const SDL_Event &event)
{
  Renderer3d::setViewport(0, 0, event.window.data1, event.window.data2);
  m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  m_camera.SetProjection(m_aspectRatio, m_fieldOfViewDegrees);
  return false;
}
} // namespace pain
