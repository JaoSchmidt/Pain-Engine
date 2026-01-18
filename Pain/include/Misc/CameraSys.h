/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


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
