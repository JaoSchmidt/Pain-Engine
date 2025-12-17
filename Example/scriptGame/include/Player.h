#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Scripting/Component.h"

namespace Player
{
reg::Entity create(pain::Scene &scene, pain::Texture &tex, glm::vec2 size,
                   int resolutionHeight, int resolutionWeigh, float zoomLevel);

class Script : public pain::OrthoCameraScript
{
public:
  void onCreate();
  using OrthoCameraScript::onEvent;
  using OrthoCameraScript::onUpdate;
  using OrthoCameraScript::OrthoCameraScript;
};

}; // namespace Player
