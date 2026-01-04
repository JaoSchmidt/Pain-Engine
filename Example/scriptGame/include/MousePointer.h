#pragma once

#include "Core.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Physics/MovementComponent.h"

namespace MousePointer
{
reg::Entity create(pain::Scene &scene);
}

class MousePointerScript : public pain::ExtendedEntity
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
