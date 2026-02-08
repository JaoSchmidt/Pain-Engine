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
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

namespace pain
{

reg::Entity Dummy3dCamera::create(pain::Scene &scene, int resolutionHeight,
                                  int resolutionWidth, float fieldOfViewDegrees)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform3dComponent{}, //
      pain::RotationComponent{},            //
      pain::Movement3dComponent{},          //
      cmp::PerspCamera::create(resolutionWidth, resolutionHeight,
                               fieldOfViewDegrees, entity), //
      pain::NativeScriptComponent{});
  pain::Scene::emplaceScript<PerspCameraScript>(entity, scene);
  return entity;
}
reg::Entity Dummy3dCamera::createBasicCamera(pain::Scene &scene,
                                             int resolutionHeight,
                                             int resolutionWeigh,
                                             float fieldOfViewDegrees)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform3dComponent{},
      Component::PerspCamera::create(resolutionWeigh, resolutionHeight,
                                     fieldOfViewDegrees, entity) //
  );
  return entity;
}

void PerspCameraScript::onCreate()
{
  m_sensitivitySpeed = 0.5f;
  m_zoomSpeed = 10.0f;
  m_cameraFront = {0.0f, 0.0f, 1.0f};
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void PerspCameraScript::onUpdate(DeltaTime deltaTimeSec)
{
  if (!m_isMovementEnable)
    return;

  auto [tc, mc, pc] = getComponents<Transform3dComponent, Movement3dComponent,
                                    cmp::PerspCamera>();
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float moveAmount = (float)(deltaTimeSec.getSecondsf() *
                             (1.0 + 10.0 * state[SDL_SCANCODE_LSHIFT]));
  glm::vec3 moveDir{0.0f};
  if (state[SDL_SCANCODE_W])
    moveDir += m_cameraFront * moveAmount;
  if (state[SDL_SCANCODE_S])
    moveDir -= m_cameraFront * moveAmount;

  if (state[SDL_SCANCODE_A])
    moveDir -= glm::cross(m_cameraFront, m_cameraUp) * moveAmount;
  if (state[SDL_SCANCODE_D])
    moveDir += glm::cross(m_cameraFront, m_cameraUp) * moveAmount;

  if (state[SDL_SCANCODE_C])
    moveDir.y += moveAmount;
  if (state[SDL_SCANCODE_SPACE])
    moveDir.y -= moveAmount;

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
  if (event.button.button == SDL_BUTTON_LEFT) {
    PLOG_I("rotation = ({},{},{})", m_cameraFront.x, m_cameraFront.y,
           m_cameraFront.z);
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

  if (m_pitch > 89.0f)
    m_pitch = 89.0f;
  if (m_pitch < -89.0f)
    m_pitch = -89.0f;

  m_cameraFront = glm::vec3(                                 //
      cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), //
      sin(glm::radians(m_pitch)),                            //
      sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))  //
  );
  auto [tc, cam] = getComponents<Transform3dComponent, cmp::PerspCamera>();
  cam.recalculateViewMatrix(tc.m_position, m_cameraFront);
}

void PerspCameraScript::onMouseScrolled(const SDL_Event &event)
{
  if (!m_isMovementEnable)
    return;
  auto cam = getComponent<cmp::PerspCamera>();

  cam.m_fieldOfViewDegrees += static_cast<float>(event.wheel.y) * m_zoomSpeed;
  if (cam.m_fieldOfViewDegrees < 1.0f)
    cam.m_fieldOfViewDegrees = 1.0f;
  else if (cam.m_fieldOfViewDegrees > 100.0f)
    cam.m_fieldOfViewDegrees = 100.0f;

  cam.setProjection(cam.m_aspectRatio, cam.m_fieldOfViewDegrees);
}

} // namespace pain
