#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct LuaSchedulerSys : public System<ComponentManager>, IOnUpdate {
  using System::System;
  void onUpdate(DeltaTime deltaTime) override;
};
} // namespace Systems

} // namespace pain
