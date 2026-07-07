/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Misc/Basic2dPlayer.h"
#include "CoreRender/CameraComponent.h"
#include "ECS/Components/NativeScript.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
#include "glm/fwd.hpp"
namespace pain
{
reg::Entity Dummy2dCamera::createMovingCamera(pain::Scene &scene,
                                              int resolutionWidth,
                                              int resolutionHeight,
                                              float zoomLevel, glm::vec2 center)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity,                             //
                         pain::Transform2dComponent{center}, //
                         pain::RotationComponent{},          //
                         pain::Movement2dComponent{},        //
                         Component::OrthoCamera::create(true, resolutionWidth,
                                                        resolutionHeight,
                                                        zoomLevel, entity), //
                         pain::NativeScriptComponent{});
  pain::Scene::emplaceScript<OrthoCameraScript>(entity, scene);
  return entity;
}
reg::Entity Dummy2dCamera::createStaticCamera(pain::Scene &scene,
                                              int resolutionWidth,
                                              int resolutionHeight,
                                              float zoomLevel, glm::vec2 center)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform2dComponent{center},
      Component::OrthoCamera::create(true, resolutionWidth, resolutionHeight,
                                     zoomLevel, entity) //
  );
  return entity;
}
void OrthoCameraScript::onMouseButtonUp(const SDL_Event &event)
{
  if (event.button.button == SDL_BUTTON_LEFT) {
    auto [tc, mc, cc] = getComponents<Transform2dComponent, Movement2dComponent,
                                      cmp::OrthoCamera>();
    PLOG_I("position = ({},{})", TP_VEC2(tc.m_position));
    PLOG_I("velocity = ({},{})", TP_VEC2(mc.m_velocity));
    PLOG_I("rotationSpeed = {}", mc.m_rotationSpeed);
    PLOG_I("playerEntity = {}", getEntity());
    PLOG_I("zoomLevel = {}", cc.m_zoomLevel);
    PLOG_I("cameraEntity = {}", cc.m_entity);
    PLOG_I("aspectRatio = {}", cc.m_aspectRatio);
    PLOG_I("resolution = ({},{})", TP_VEC2(cc.m_resolution));
  }
}

void OrthoCameraScript::onUpdate(DeltaTime deltaTime)
{
  if (hasAnyComponents<Movement2dComponent, Transform2dComponent>()) {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    auto [mc, tc, cc, rc] =
        getComponents<Movement2dComponent, Transform2dComponent,
                      Component::OrthoCamera, RotationComponent>();

    glm::vec3 moveDir{0.0F};

    if (state[SDL_SCANCODE_W])
      moveDir -= glm::cross(rc.m_rotation, {0.0F, 0.0F, 1.0F});
    if (state[SDL_SCANCODE_S])
      moveDir += glm::cross(rc.m_rotation, {0.0F, 0.0F, 1.0F});
    if (state[SDL_SCANCODE_A])
      moveDir -= rc.m_rotation;
    if (state[SDL_SCANCODE_D])
      moveDir += rc.m_rotation;

    // Normalize movement direction (avoid diagonal speed boost)
    if (glm::length(moveDir) > 0.0001F)
      moveDir = glm::normalize(moveDir);

    float moveSpeed = cc.m_zoomLevel * (1.0F + state[SDL_SCANCODE_LSHIFT]);
    mc.m_velocity = moveDir * moveSpeed;

    if (state[SDL_SCANCODE_Q])
      rc.m_rotationRadians += mc.m_rotationSpeed * deltaTime.getSecondsf();
    if (state[SDL_SCANCODE_E])
      rc.m_rotationRadians -= mc.m_rotationSpeed * deltaTime.getSecondsf();

    tc.m_position += mc.m_velocity * deltaTime.getSecondsf();
    cc.recalculateViewMatrix(tc.m_position, rc.m_rotationRadians);
  } else { // TODO: finish 3d ortho version
    auto [mc, tc, cc] = getComponents<Movement3dComponent, Transform3dComponent,
                                      Component::OrthoCamera>();
    cc.recalculateViewMatrix(glm::vec2(tc.m_position), 0);
  }
}

void OrthoCameraScript::onEvent(const SDL_Event &event)
{
  switch (event.type) {
  case SDL_MOUSEBUTTONUP:
    onMouseButtonUp(event);
    break;
  case SDL_MOUSEWHEEL:
    onMouseScrolled(event);
    break;
  default:
    break;
  }
}

void OrthoCameraScript::onMouseScrolled(const SDL_Event &event)
{
  Component::OrthoCamera &cc = getComponent<Component::OrthoCamera>();
  cc.m_zoomLevel -= (float)event.wheel.y * m_zoomSpeed;
  cc.m_zoomLevel = std::max(cc.m_zoomLevel, 0.25F);
  cc.setProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                   cc.m_aspectRatio * cc.m_zoomLevel, -cc.m_zoomLevel,
                   cc.m_zoomLevel);
}

} // namespace pain
