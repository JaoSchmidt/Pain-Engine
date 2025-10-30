#include "Misc/BasicOrthoCamera.h"
#include "CoreRender/Camera.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Components/Rotation.h"
#include "glm/fwd.hpp"
namespace pain
{

OrthoCamera::OrthoCamera(Scene *scene, float aspectRatio, float zoomLevel)
    : NormalEntity(*scene)
{
  // clang-format off
  createComponents(*scene,
      MovementComponent{},
      RotationComponent{},
      TransformComponent{},
      OrthoCameraComponent{aspectRatio, zoomLevel},
      NativeScriptComponent{}
  );
  // clang-format on
};
// OrthoCameraScript inherits ExtendedEntity
inline void OrthoCameraScript::recalculatePosition(const glm::vec3 &position,
                                                   const float rotation)
{
  getComponent<OrthoCameraComponent>().m_camera->RecalculateViewMatrix(
      position, rotation);
}

void OrthoCameraScript::onUpdate(double deltaTimeSec)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  auto [mc, tc, cc, rc] =
      getComponents<MovementComponent, TransformComponent, OrthoCameraComponent,
                    RotationComponent>();

  glm::vec3 moveDir{0.0f};

  if (state[SDL_SCANCODE_W])
    moveDir += -glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f});
  if (state[SDL_SCANCODE_S])
    moveDir += glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f});
  if (state[SDL_SCANCODE_A])
    moveDir += -rc.m_rotation;
  if (state[SDL_SCANCODE_D])
    moveDir += rc.m_rotation;

  // Normalize movement direction (avoid diagonal speed boost)
  if (glm::length(moveDir) > 0.0001f)
    moveDir = glm::normalize(moveDir);

  float moveSpeed = cc.m_zoomLevel * (1.0f + state[SDL_SCANCODE_LSHIFT]);
  mc.m_velocity = moveDir * moveSpeed;

  if (state[SDL_SCANCODE_Q])
    rc.m_rotationAngle += mc.m_rotationSpeed * static_cast<float>(deltaTimeSec);
  if (state[SDL_SCANCODE_E])
    rc.m_rotationAngle -= mc.m_rotationSpeed * static_cast<float>(deltaTimeSec);

  tc.m_position += mc.m_velocity * static_cast<float>(deltaTimeSec);
  recalculatePosition(tc.m_position, rc.m_rotationAngle);
}

void OrthoCameraScript::onEvent(const SDL_Event &event)
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

void OrthoCameraScript::onMouseScrolled(const SDL_Event &event,
                                        OrthoCameraComponent &cc)
{
  cc.m_zoomLevel -= (float)event.wheel.y * m_zoomSpeed;
  cc.m_zoomLevel = std::max(cc.m_zoomLevel, 0.25f);
  cc.m_camera->SetProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                             cc.m_aspectRatio * cc.m_zoomLevel, -cc.m_zoomLevel,
                             cc.m_zoomLevel);
}

void OrthoCameraScript::onWindowResized(const SDL_Event &event,
                                        OrthoCameraComponent &cc)
{
  cc.m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  cc.m_camera->SetProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                             cc.m_aspectRatio * cc.m_zoomLevel, -cc.m_zoomLevel,
                             cc.m_zoomLevel);
}
} // namespace pain
