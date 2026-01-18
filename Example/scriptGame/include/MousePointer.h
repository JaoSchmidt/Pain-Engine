/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Physics/MovementComponent.h"

namespace MousePointer
{
reg::Entity create(pain::Scene &scene);
}

class MousePointerScript : public pain::WorldObject
{
public:
  MousePointerScript(reg::Entity entity, pain::Scene &scene,
                     reg::Entity m_cameraEntity);
  void onEvent(const SDL_Event &e);
  void onUpdate(pain::DeltaTime deltaTime);
  void onCreate();

private:
  reg::Entity m_cameraEntity = reg::Entity{-2};
  glm::vec2 screenToWorld(int mouseX, int mouseY);
};
