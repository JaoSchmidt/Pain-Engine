/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


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
