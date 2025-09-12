#pragma once

#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct Render : public System {
  using System::System;
  void onRender(Renderer2d &renderer, bool isMinimized, double currentTime);
};
} // namespace Systems
} // namespace pain
