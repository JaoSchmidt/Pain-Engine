/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Assets/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/RotationComponent.h"
#include "glm/geometric.hpp"
#include "platform/ContextBackend.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

namespace pain
{

/** Yaw and Pitch are in degrees*/
reg::Entity Dummy3dCamera::create(pain::Scene &scene, int resolutionWidth,
                                  int resolutionHeight,
                                  float fieldOfViewDegrees, float yaw,
                                  float pitch)
{
  reg::Entity entity = scene.createEntity("Moving3DCamera");
  scene.createComponents(
      entity, pain::Transform3dComponent{}, //
      pain::RotationComponent{},            //
      pain::Movement3dComponent{},          //
      PerspCameraComponent::create(true, resolutionWidth, resolutionHeight,
                               fieldOfViewDegrees, entity, yaw, pitch), //
      pain::NativeScriptComponent{});
  pain::Scene::emplaceScript<PerspCameraScript>(entity, scene, yaw, pitch);
  return entity;
}
reg::Entity Dummy3dCamera::createBasicCamera(pain::Scene &scene,
                                             int resolutionWidth,
                                             int resolutionHeight,
                                             float fieldOfViewDegrees,
                                             float yaw, float pitch)
{
  reg::Entity entity = scene.createEntity("Static3DCamera");
  scene.createComponents(
      entity, pain::Transform3dComponent{},
      PerspCameraComponent::create(true, resolutionWidth, resolutionHeight,
                                     fieldOfViewDegrees, entity, yaw, pitch) //
  );
  return entity;
}
void PerspCameraScript::onCreate()
{
  m_sensitivitySpeed = 0.5F;
  m_zoomSpeed = 10.0F;
  m_cameraFront = {0.0F, 0.0F, 1.0F};

  auto [tc, mc, pc] = getComponents<Transform3dComponent, Movement3dComponent,
                                    PerspCameraComponent>();
  m_cameraFront = glm::vec3(                                 //
      cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), //
      sin(glm::radians(m_pitch)),                            //
      sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))  //
  );
  pc.recalculateViewMatrix(tc.m_position, m_cameraFront);

  PLOG_I("cameraFront = ({},{},{})", TP_VEC3(m_cameraFront));
  PLOG_I("yaw, pitch, roll = ({},{},{})", m_yaw, m_pitch, 0);
  PLOG_I("position = ({},{},{})", TP_VEC3(tc.m_position));
  PLOG_I("velocity = ({},{},{})", TP_VEC3(mc.m_velocity));
  PLOG_I("rotationSpeed = {}", mc.m_rotationSpeed);
  PLOG_I("playerEntity = {}", getEntity());
  setMovementState<false>();
}

// PerspCameraScript::PerspCameraScript(reg::Entity entity, Scene &scene)
//     : WorldObject(entity, scene) {};
PerspCameraScript::PerspCameraScript(reg::Entity entity, Scene &scene,
                                     float yaw, float pitch)
    : WorldObject(entity, scene), m_yaw(yaw), m_pitch(pitch) {};

void PerspCameraScript::onUpdate(DeltaTime deltaTimeSec)
{
  if (!m_isMovementEnable)
    return;

  auto [tc, mc, pc] = getComponents<Transform3dComponent, Movement3dComponent,
                                    PerspCameraComponent>();
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float moveAmount = (float)(deltaTimeSec.getSecondsf() *
                             (1.0 + 10.0 * state[SDL_SCANCODE_LSHIFT]));
  glm::vec3 moveDir{0.0F};
  if (state[SDL_SCANCODE_W])
    moveDir += m_cameraFront * moveAmount;
  if (state[SDL_SCANCODE_S])
    moveDir -= m_cameraFront * moveAmount;

  if (state[SDL_SCANCODE_A])
    moveDir += glm::cross(m_cameraFront, m_cameraUp) * moveAmount;
  if (state[SDL_SCANCODE_D])
    moveDir -= glm::cross(m_cameraFront, m_cameraUp) * moveAmount;

  if (state[SDL_SCANCODE_C])
    moveDir.y -= moveAmount;
  if (state[SDL_SCANCODE_SPACE])
    moveDir.y += moveAmount;

  mc.m_velocity = moveDir * m_moveSpeed;
  pc.recalculateViewMatrix(tc.m_position, m_cameraFront);
}

template <bool IsMoving> void PerspCameraScript::setMovementState()
{
  constexpr SDL_bool relativeMouseMode = IsMoving ? SDL_TRUE : SDL_FALSE;
  SDL_SetRelativeMouseMode(relativeMouseMode);
  m_isMovementEnable = IsMoving;
}

void PerspCameraScript::onMouseButtonUp(const SDL_Event &event)
{
  auto [tc, mc, pc] = getComponents<Transform3dComponent, Movement3dComponent,
                                    PerspCameraComponent>();
  if (event.button.button == SDL_BUTTON_LEFT) {
    PLOG_I("cameraFront = ({},{},{})", TP_VEC3(m_cameraFront));
    PLOG_I("yaw, pitch, roll = ({},{},{})", m_yaw, m_pitch, 0);
    PLOG_I("position = ({},{},{})", TP_VEC3(tc.m_position));
    PLOG_I("velocity = ({},{},{})", TP_VEC3(mc.m_velocity));
    PLOG_I("rotationSpeed = {}", mc.m_rotationSpeed);
    PLOG_I("resolution = ({},{})", TP_VEC2(pc.m_resolution));
    PLOG_I("aspectRatio = {}", pc.m_aspectRatio);
    PLOG_I("fov = {}", pc.m_fieldOfViewDegrees);
  }
}

void PerspCameraScript::onEvent(const SDL_Event &event)
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
  default:
    break;
  }
}

void PerspCameraScript::onMouseMoved(const SDL_Event &e)
{
  if (!m_isMovementEnable)
    return;

  float xoffset = static_cast<float>(e.motion.xrel) * m_sensitivitySpeed;
  float yoffset = static_cast<float>(e.motion.yrel) * m_sensitivitySpeed;

  m_yaw += xoffset;
  m_pitch -= yoffset;

  if (m_pitch > 89.0F)
    m_pitch = 89.0F;
  if (m_pitch < -89.0F)
    m_pitch = -89.0F;

  m_cameraFront = glm::vec3(                                 //
      cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), //
      sin(glm::radians(m_pitch)),                            //
      sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))  //
  );
  auto [tc, cam] = getComponents<Transform3dComponent, PerspCameraComponent>();
  cam.recalculateViewMatrix(tc.m_position, m_cameraFront);
}

void PerspCameraScript::onMouseScrolled(const SDL_Event &event)
{
  if (!m_isMovementEnable)
    return;
  auto cam = getComponent<PerspCameraComponent>();

  cam.m_fieldOfViewDegrees += static_cast<float>(event.wheel.y) * m_zoomSpeed;
  if (cam.m_fieldOfViewDegrees < 1.0F)
    cam.m_fieldOfViewDegrees = 1.0F;
  else if (cam.m_fieldOfViewDegrees > 100.0F)
    cam.m_fieldOfViewDegrees = 100.0F;

  cam.setProjection(cam.m_aspectRatio, cam.m_fieldOfViewDegrees);
}

} // namespace pain
