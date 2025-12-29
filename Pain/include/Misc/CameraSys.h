#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct CameraSys : public System<ComponentManager>, IOnEvent {
  using System::System;
  void onEvent(const SDL_Event &event) override;
};
} // namespace Systems
} // namespace pain
