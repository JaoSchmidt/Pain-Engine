#pragma once
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct Kinematics : public System<ComponentManager> {
  using System::System;
  void onUpdate(double dt);
};
} // namespace Systems
} // namespace pain
