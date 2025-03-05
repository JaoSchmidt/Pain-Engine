#include "Misc/BasicOrthoCamera.h"
#include "CoreRender/Camera.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "glm/fwd.hpp"

#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"
namespace pain
{

OrthoCameraEntity::OrthoCameraEntity(Scene *scene, float aspectRatio,
                                     float zoomLevel)
    : GameObject(scene)
{
  addComponents(MovementComponent{}, RotationComponent{}, TransformComponent{},
                OrthoCameraComponent{aspectRatio, zoomLevel});
};

inline void
OrthoCameraController::recalculatePosition(const glm::vec3 &position,
                                           const float rotation)
{
  getComponent<OrthoCameraComponent>().m_camera->RecalculateViewMatrix(
      position, rotation);
}

void OrthoCameraController::onUpdate(double deltaTimeSec)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  auto [mc, tc, cc, rc] = getAllComponents();

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

  mc.m_translationSpeed = cc.m_zoomLevel * (1.0f + state[SDL_SCANCODE_LSHIFT]);
  recalculatePosition(tc.m_position, rc.m_rotationAngle);
}

void OrthoCameraController::onEvent(const SDL_Event &event)
{
  OrthoCameraComponent &cc = getComponent<OrthoCameraComponent>();
  if (event.type == SDL_MOUSEWHEEL)
    onMouseScrolled(event, cc);
  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      onWindowResized(event, cc);
    }
  }
}

bool OrthoCameraController::onMouseScrolled(const SDL_Event &event,
                                            OrthoCameraComponent &cc)
{
  cc.m_zoomLevel -= event.wheel.y * 0.25f;
  cc.m_zoomLevel = std::max(cc.m_zoomLevel, 0.25f);
  cc.m_camera->SetProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                             cc.m_aspectRatio * cc.m_zoomLevel, -cc.m_zoomLevel,
                             cc.m_zoomLevel);
  return false;
}

bool OrthoCameraController::onWindowResized(const SDL_Event &event,
                                            OrthoCameraComponent &cc)
{
  Renderer2d::setViewport(0, 0, event.window.data1, event.window.data2);
  cc.m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  cc.m_camera->SetProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                             cc.m_aspectRatio * cc.m_zoomLevel, -cc.m_zoomLevel,
                             cc.m_zoomLevel);
  return false;
}
} // namespace pain
