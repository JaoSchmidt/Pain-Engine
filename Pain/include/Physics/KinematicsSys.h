#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct Kinematics : public System<ComponentManager> {
  using System::System;
  void onUpdate(DeltaTime dt);
};
} // namespace Systems
} // namespace pain
