#include "Misc/BasicOrthoCamera.h"
#include "CoreRender/Camera.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"
#include "ECS/GameObject.h"
#include "glm/fwd.hpp"

namespace pain
{
OrthoCameraEntity::OrthoCameraEntity(Scene *scene, float aspectRatio,
                                     float zoomLevel)
    : GameObject(scene), m_aspectRatio(aspectRatio), m_zoomLevel(zoomLevel),
      m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel,
               -m_zoomLevel, m_zoomLevel)
{
  addComponent<MovementComponent>();
  addComponent<RotationComponent>();
  addComponent<TransformComponent>();
};

void OrthoCameraController::onCreate() {};
void OrthoCameraController::onDestroy() {};

inline void
OrthoCameraController::recalculatePosition(const glm::vec3 &position,
                                           const float rotation)
{
  getComponent<OrthographicCamera>().RecalculateViewMatrix(position, rotation);
}

void OrthoCameraController::onUpdate(double deltaTimeSec)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  MovementComponent &mc = getComponent<MovementComponent>();
  RotationComponent &rc = getComponent<RotationComponent>();
  const TransformComponent &tc = getComponent<TransformComponent>();

  mc.m_velocityDir =
      (state[SDL_SCANCODE_W] ? -glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f})
                             : glm::vec3(0.0)) +
      (state[SDL_SCANCODE_S] ? glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f})
                             : glm::vec3(0.0)) +
      (state[SDL_SCANCODE_A] ? -rc.m_rotation : glm::vec3(0.0)) +
      (state[SDL_SCANCODE_D] ? rc.m_rotation : glm::vec3(0.0));

  if (state[SDL_SCANCODE_Q])
    rc.m_rotationAngle += mc.m_rotationSpeed * deltaTimeSec;
  if (state[SDL_SCANCODE_E])
    rc.m_rotationAngle -= mc.m_rotationSpeed * deltaTimeSec;
  // PLOG_I("({},{},{})", tc.m_position.x, tc.m_position.y, tc.m_position.z);

  const OrthoCameraEntity cam = getEntity<OrthoCameraEntity>();
  mc.m_translationSpeed = cam.m_zoomLevel * (1.0f + state[SDL_SCANCODE_LSHIFT]);
  recalculatePosition(tc.m_position, rc.m_rotationAngle);
}

void OrthoCameraController::onEvent(const SDL_Event &event)
{
  if (event.type == SDL_MOUSEWHEEL)
    onMouseScrolled(event);
  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      onWindowResized(event);
    }
  }
}

bool OrthoCameraController::onMouseScrolled(const SDL_Event &event)
{
  OrthoCameraEntity cam = getEntity<OrthoCameraEntity>();
  cam.m_zoomLevel -= event.wheel.y * 0.25f;
  cam.m_zoomLevel = std::max(cam.m_zoomLevel, 0.25f);
  cam.getCamera().SetProjection(-cam.m_aspectRatio * cam.m_zoomLevel,
                                cam.m_aspectRatio * cam.m_zoomLevel,
                                -cam.m_zoomLevel, cam.m_zoomLevel);
  return false;
}

bool OrthoCameraController::onWindowResized(const SDL_Event &event)
{
  Renderer2d::setViewport(0, 0, event.window.data1, event.window.data2);
  OrthoCameraEntity cam = getEntity<OrthoCameraEntity>();
  cam.m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  cam.getCamera().SetProjection(-cam.m_aspectRatio * cam.m_zoomLevel,
                                cam.m_aspectRatio * cam.m_zoomLevel,
                                -cam.m_zoomLevel, cam.m_zoomLevel);
  return false;
}
} // namespace pain
