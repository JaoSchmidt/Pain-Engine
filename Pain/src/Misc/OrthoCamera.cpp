#include "Misc/OrthoCamera.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/Movement.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

namespace pain
{

OrthographicCameraEntity::OrthographicCameraEntity(float aspectRatio)
    : m_aspectRatio(aspectRatio), m_zoomLevel(1.0f),
      m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel,
               -m_zoomLevel, m_zoomLevel)
{
  addComponent<MovementComponent>(0.0f, 0.0f, 0.0f);
  m_position = {0.0f, 0.0f, 0.0f};
  m_rotation = 0.0f;
  m_translationSpeed = 1.0f;
  m_rotationSpeed = 180.0f;
  m_zoomSpeed = 0.25f;
}

void OrthographicCameraEntity::onUpdate(double deltaTimeSec)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);

  if (state[SDL_SCANCODE_A])
    m_position.x -= m_translationSpeed * deltaTimeSec;
  if (state[SDL_SCANCODE_D])
    m_position.x += m_translationSpeed * deltaTimeSec;

  if (state[SDL_SCANCODE_W])
    m_position.y += m_translationSpeed * deltaTimeSec;
  if (state[SDL_SCANCODE_S])
    m_position.y -= m_translationSpeed * deltaTimeSec;

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

void OrthographicCameraEntity::onEvent(const SDL_Event &event)
{
  if (event.type == SDL_MOUSEWHEEL)
    onMouseScrolled(event);
  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      onWindowResized(event);
    }
  }
}

bool OrthographicCameraEntity::onMouseScrolled(const SDL_Event &event)
{
  m_zoomLevel -= event.wheel.y * 0.25f;
  m_zoomLevel = std::max(m_zoomLevel, 0.25f);
  m_camera.SetProjection(-m_aspectRatio * m_zoomLevel,
                         m_aspectRatio * m_zoomLevel, -m_zoomLevel,
                         m_zoomLevel);
  return false;
}

bool OrthographicCameraEntity::onWindowResized(const SDL_Event &event)
{
  Renderer2d::setViewport(0, 0, event.window.data1, event.window.data2);
  m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  m_camera.SetProjection(-m_aspectRatio * m_zoomLevel,
                         m_aspectRatio * m_zoomLevel, -m_zoomLevel,
                         m_zoomLevel);
  return false;
}
} // namespace pain
