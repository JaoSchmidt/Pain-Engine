/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


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
