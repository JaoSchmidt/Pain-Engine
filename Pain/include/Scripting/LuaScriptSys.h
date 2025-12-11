#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Systems.h"

namespace pain
{
struct Renderer2d;
namespace Systems
{
struct LuaScript : public System<ComponentManager> {
  using System::System;
  void onUpdate(DeltaTime deltaTime);
  void onEvent(const SDL_Event &e);
  void onRender(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime);
};
} // namespace Systems

} // namespace pain
