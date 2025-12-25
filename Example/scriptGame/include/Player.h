#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Scriptable.h"
#include "Misc/BasicOrthoCamera.h"

namespace Player
{
reg::Entity create(pain::Scene &scene, pain::Texture &tex, glm::vec2 size,
                   int resolutionHeight, int resolutionWeigh, float zoomLevel);

class Script : public pain::OrthoCameraScript
{
public:
  void onRender(pain::Renderer2d &renderer, bool isMinimized,
                pain::DeltaTime currentTime);
  void onCreate();
  void onUpdate(pain::DeltaTime deltaTime) = delete;
  using OrthoCameraScript::onEvent;
  using OrthoCameraScript::OrthoCameraScript;
};

}; // namespace Player
