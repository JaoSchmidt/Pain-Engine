#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"

#include "ECS/Systems.h"

namespace pain
{
namespace cmp
{
struct LuaScheduleTask;
}

namespace Systems
{
struct LuaSchedulerSys : public System<WorldComponents>, IOnUpdate {
  using Tags = TypeList<cmp::LuaScheduleTask>; //
  using System::System;
  void onUpdate(DeltaTime deltaTime) override;
};
} // namespace Systems

} // namespace pain
