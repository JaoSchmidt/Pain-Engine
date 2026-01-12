#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"
#include "Physics/MovementComponent.h"

namespace pain
{
namespace Systems
{
struct Kinematics : public System<WorldComponents>, IOnUpdate {
  using Tags = TypeList<Transform2dComponent, Movement2dComponent>;
  using System<WorldComponents>::System;
  Kinematics() = delete;
  void onUpdate(DeltaTime deltaTime) override;
};
} // namespace Systems
} // namespace pain
