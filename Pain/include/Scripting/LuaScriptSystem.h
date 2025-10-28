#pragma once
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct LuaScript : public System<ComponentManager> {
  using System::System;
  void onUpdate(double deltaTime);
  void onEvent(const SDL_Event &e);
  void onRender(Renderer2d &renderer, bool isMinimized, double currentTime);
};
} // namespace Systems

} // namespace pain
