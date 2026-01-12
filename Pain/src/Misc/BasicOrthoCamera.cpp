#include "Misc/BasicOrthoCamera.h"
#include "CoreRender/CameraComponent.h"
#include "ECS/Components/NativeScript.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
#include "glm/fwd.hpp"
namespace pain
{
reg::Entity Dummy2dCamera::create(pain::Scene &scene, int resolutionHeight,
                                  int resolutionWidth, float zoomLevel)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, pain::Transform2dComponent{}, //
                         pain::RotationComponent{},            //
                         pain::Movement2dComponent{},          //
                         Component::OrthoCamera::create(
                             resolutionWidth, resolutionHeight, zoomLevel), //
                         pain::NativeScriptComponent{});
  pain::Scene::emplaceScript<OrthoCameraScript>(entity, scene);
  return entity;
}
reg::Entity Dummy2dCamera::createBasicCamera(pain::Scene &scene,
                                             int resolutionHeight,
                                             int resolutionWeigh,
                                             float zoomLevel)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, pain::Transform2dComponent{},
                         Component::OrthoCamera::create(
                             resolutionWeigh, resolutionHeight, zoomLevel) //
  );
  return entity;
}

void OrthoCameraScript::onCreate()
{
  getComponent<Movement2dComponent>().m_rotationSpeed = 1.f;
}

void OrthoCameraScript::onUpdate(DeltaTime deltaTime)
{
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  auto [mc, tc, cc, rc] =
      getComponents<Movement2dComponent, Transform2dComponent,
                    Component::OrthoCamera, RotationComponent>();

  glm::vec3 moveDir{0.0f};

  if (state[SDL_SCANCODE_W])
    moveDir += glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f});
  if (state[SDL_SCANCODE_S])
    moveDir += -glm::cross(rc.m_rotation, {0.0f, 0.0f, 1.0f});
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
    rc.m_rotationAngle += mc.m_rotationSpeed * deltaTime.getSecondsf();
  if (state[SDL_SCANCODE_E])
    rc.m_rotationAngle -= mc.m_rotationSpeed * deltaTime.getSecondsf();

  tc.m_position += mc.m_velocity * deltaTime.getSecondsf();
  cc.recalculateViewMatrix(tc.m_position, rc.m_rotationAngle);
}

void OrthoCameraScript::onEvent(const SDL_Event &event)
{
  Component::OrthoCamera &cc = getComponent<Component::OrthoCamera>();
  if (event.type == SDL_MOUSEWHEEL)
    onMouseScrolled(event, cc);
}

void OrthoCameraScript::onMouseScrolled(const SDL_Event &event,
                                        Component::OrthoCamera &cc)
{
  cc.m_zoomLevel -= (float)event.wheel.y * m_zoomSpeed;
  cc.m_zoomLevel = std::max(cc.m_zoomLevel, 0.25f);
  cc.setProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                   cc.m_aspectRatio * cc.m_zoomLevel, -cc.m_zoomLevel,
                   cc.m_zoomLevel);
}

} // namespace pain
