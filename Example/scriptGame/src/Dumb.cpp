/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Dumb.h"

reg::Entity DumbObject::create(pain::Scene &scene)
{
  reg::Entity entity = scene.createEntity();

  scene.createComponents(entity, pain::Transform3dComponent{},
                         pain::Movement3dComponent{},
                         pain::NativeScriptComponent{});

  pain::Scene::emplaceScript<DumbObjectScript>(entity, scene);
  return entity;
}

void DumbObjectScript::onCreate() { m_moveSpeed = 20.0f; }

void DumbObjectScript::onUpdate(pain::DeltaTime deltaTimeSec)
{
  auto [tc, mc] =
      getComponents<pain::Transform3dComponent, pain::Movement3dComponent>();

  const Uint8 *state = SDL_GetKeyboardState(NULL);

  float moveAmount =
      deltaTimeSec.getSecondsf() * (1.0f + 10.0f * state[SDL_SCANCODE_LSHIFT]);

  glm::vec3 moveDir{0.0f};

  // -------- Arrow Keys --------
  if (state[SDL_SCANCODE_UP])
    moveDir.z += moveAmount;

  if (state[SDL_SCANCODE_DOWN])
    moveDir.z -= moveAmount;

  if (state[SDL_SCANCODE_LEFT])
    moveDir.x -= moveAmount;

  if (state[SDL_SCANCODE_RIGHT])
    moveDir.x += moveAmount;

  // Vertical movement
  if (state[SDL_SCANCODE_K])
    moveDir.y += moveAmount;

  if (state[SDL_SCANCODE_J])
    moveDir.y -= moveAmount;

  mc.m_velocity = moveDir * m_moveSpeed;
}
