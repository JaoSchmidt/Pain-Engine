#pragma once

#include "Core.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Physics/Collision/GridManager.h"

class MousePointer : public pain::NormalEntity<pain::TransformComponent, //
                                               pain::SpriteComponent,    //
                                               pain::NativeScriptComponent>
{
public:
  MousePointer(pain::Scene &scene);
};

class MousePointerScript : public pain::ExtendedEntity
{
public:
  MousePointerScript(reg::Entity entity, pain::Scene &scene,
                     reg::Entity m_cameraEntity);
  void onEvent(const SDL_Event &e);
  void onUpdate(double deltaTimeSec);
  void onCreate();

private:
  int m_eventcounter = 0;
  int m_updatecounter = 0;
  reg::Entity m_cameraEntity = reg::Entity{-2};
  glm::vec2 screenToWorld(int mouseX, int mouseY);
};
