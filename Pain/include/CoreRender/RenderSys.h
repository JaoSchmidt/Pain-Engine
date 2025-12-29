#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
struct Renderer2d;
namespace Systems
{
struct Render : public System<ComponentManager>, IOnRender {
  using System::System;
  void onRender(Renderer2d &renderer, bool isMinimized,
                DeltaTime currentTime) override;
};
} // namespace Systems
} // namespace pain
