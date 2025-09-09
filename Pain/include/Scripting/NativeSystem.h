#pragma once
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct NativeScript : public System {
  using System::System;
  void onUpdate(double deltaTime);
  void onEvent(const SDL_Event &e);
  void onRender(bool isMinimized, double currentTime);
};
} // namespace Systems

} // namespace pain
