/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BasicOrthoCamera.h
#pragma once

#include "Core.h"
#include "CoreRender/CameraComponent.h"
#include "ECS/Scriptable.h"
#include <type_traits>

namespace pain
{

namespace Dummy2dCamera
{
reg::Entity create(pain::Scene &scene, int resolutionHeight,
                   int resolutionWidth, float zoomLevel);

reg::Entity createBasicCamera(pain::Scene &scene, int resolutionHeight,
                              int resolutionWidth, float zoomLevel);
} // namespace Dummy2dCamera

class OrthoCameraScript : public WorldObject
{
public:
  using WorldObject::WorldObject;
  void onUpdate(DeltaTime deltaTimeSec);
  void onEvent(const SDL_Event &e);

protected:
  float m_zoomSpeed = 0.25f;
  void onMouseScrolled(const SDL_Event &e, Component::OrthoCamera &cc);
  void onWindowResized(const SDL_Event &e, Component::OrthoCamera &cc);
};

} // namespace pain
