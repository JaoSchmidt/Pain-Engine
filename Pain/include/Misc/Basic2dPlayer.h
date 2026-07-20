/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Basic2dPlayer.h
#pragma once

#include "Core.h"
#include "CoreRender/CameraComponent.h"
#include "ECS/Scriptable.h"
#include <type_traits>

namespace pain
{

namespace Dummy2dCamera
{
reg::Entity createMovingCamera(pain::Scene &scene, int resolutionWidth,
                               int resolutionHeight, float zoomLevel,
                               glm::vec2 center = {0, 0});

reg::Entity createStaticCamera(pain::Scene &scene, int resolutionWidth,
                               int resolutionHeight, float zoomLevel,
                               glm::vec2 center = {0, 0});
} // namespace Dummy2dCamera

class OrthoCameraScript : public WorldObject
{
public:
  using WorldObject::WorldObject;
  void onUpdate(DeltaTime deltaTimeSec);
  void onEvent(const SDL_Event &e);

  glm::vec2 screenToWorld(int mouseX, int mouseY,
                          const Transform2dComponent &camTC,
                          const RotationComponent &camRC);

protected:
  float m_zoomSpeed = 0.25f;
  void onMouseScrolled(const SDL_Event &e);
  void onMouseButtonUp(const SDL_Event &event);
};

} // namespace pain
