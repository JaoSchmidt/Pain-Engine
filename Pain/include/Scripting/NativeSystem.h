#pragma once
#include "ECS/Systems.h"

namespace pain
{
struct Renderer2d;

namespace Systems
{
struct NativeScript : public System<ComponentManager> {
  using System::System;
  void onUpdate(double deltaTime);
  void onEvent(const SDL_Event &e);
  void onRender(Renderer2d &renderer, bool isMinimized, double currentTime);
};
} // namespace Systems

} // namespace pain
