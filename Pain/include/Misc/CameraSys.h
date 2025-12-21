#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct CameraSys : public System<ComponentManager> {
  using System::System;
  void onEvent(const SDL_Event &event);
};
} // namespace Systems
} // namespace pain
