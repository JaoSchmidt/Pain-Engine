/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Scriptable.h"
#include "Misc/BasicOrthoCamera.h"

namespace Player3d
{
reg::Entity create(pain::Scene &scene, pain::Texture &tex, glm::vec3 initialPos,
                   glm::vec3 size, int resolutionHeight, int resolutionWeigh,
                   float zoomLevel);

class Script : public pain::OrthoCameraScript
{
public:
  void onRender(pain::Renderers &renderer, bool isMinimized,
                pain::DeltaTime currentTime);
  void onCreate();
  using OrthoCameraScript::onEvent;
  using OrthoCameraScript::OrthoCameraScript;
  using pain::OrthoCameraScript::onUpdate;

private:
  void onMouseScrolled(const SDL_Event &e, Component::OrthoCamera &cc);
};

}; // namespace Player3d
