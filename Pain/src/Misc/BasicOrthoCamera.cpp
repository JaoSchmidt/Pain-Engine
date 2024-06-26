#include "Misc/BasicOrthoCamera.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/Movement.h"
#include "glm/fwd.hpp"

namespace pain
{

OrthoCameraEntity::OrthoCameraEntity(Scene *scene, float aspectRatio)
    : GameObjWrapper(scene), m_aspectRatio(aspectRatio), m_zoomLevel(1.0f),
      m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel,
               -m_zoomLevel, m_zoomLevel)
{
  addComponent<MovementComponent>();
  addComponent<RotationComponent>();
  addComponent<TransformComponent>();
  m_zoomSpeed = 0.25f;
}

inline void OrthoCameraEntity::recalculatePosition(const glm::vec3 &position,
                                                   const float rotation)
{
  m_camera.RecalculateViewMatrix(position, rotation);
}

void OrthoCameraEntity::onUpdate(double deltaTimeSec)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  MovementComponent &mc = m_scene->getComponent<MovementComponent>(m_entity);
  RotationComponent &rc = m_scene->getComponent<RotationComponent>(m_entity);
  const TransformComponent &tc =
      m_scene->getComponent<TransformComponent>(m_entity);

  mc.m_velocityDir =
      (state[SDL_SCANCODE_W] ? glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f})
                             : glm::vec3(0.0)) +
      (state[SDL_SCANCODE_S] ? -glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f})
                             : glm::vec3(0.0)) +
      (state[SDL_SCANCODE_A] ? -rc.m_rotation : glm::vec3(0.0)) +
      (state[SDL_SCANCODE_D] ? rc.m_rotation : glm::vec3(0.0));

  if (state[SDL_SCANCODE_Q])
    rc.m_rotationAngle += mc.m_rotationSpeed * deltaTimeSec;
  if (state[SDL_SCANCODE_E])
    rc.m_rotationAngle -= mc.m_rotationSpeed * deltaTimeSec;
  // PLOG_I("({},{},{})", tc.m_position.x, tc.m_position.y, tc.m_position.z);
  mc.m_translationSpeed = m_zoomLevel * (1.0f + state[SDL_SCANCODE_LSHIFT]);
  recalculatePosition(tc.m_position, rc.m_rotationAngle);
}

void OrthoCameraEntity::onEvent(const SDL_Event &event)
{
  if (event.type == SDL_MOUSEWHEEL)
    onMouseScrolled(event);
  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      onWindowResized(event);
    }
  }
}

bool OrthoCameraEntity::onMouseScrolled(const SDL_Event &event)
{
  m_zoomLevel -= event.wheel.y * 0.25f;
  m_zoomLevel = std::max(m_zoomLevel, 0.25f);
  m_camera.SetProjection(-m_aspectRatio * m_zoomLevel,
                         m_aspectRatio * m_zoomLevel, -m_zoomLevel,
                         m_zoomLevel);
  return false;
}

bool OrthoCameraEntity::onWindowResized(const SDL_Event &event)
{
  Renderer2d::setViewport(0, 0, event.window.data1, event.window.data2);
  m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  m_camera.SetProjection(-m_aspectRatio * m_zoomLevel,
                         m_aspectRatio * m_zoomLevel, -m_zoomLevel,
                         m_zoomLevel);
  return false;
}
} // namespace pain
