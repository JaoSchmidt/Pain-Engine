#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace cmp
{
struct OrthoCamera;
}

namespace pain
{

namespace Systems
{
struct CameraSys : public System<WorldComponents>, IOnEvent {
  using Tags = TypeList<::cmp::OrthoCamera>;
  using System::System;
  void onEvent(const SDL_Event &event) override;
};
} // namespace Systems
} // namespace pain
