/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include "Core.h"
#include "ECS/Components/ComponentManager.h"
#include <SDL2/SDL_events.h>

#include <sol/sol.hpp>
namespace pain
{
namespace cmp
{

struct LuaScheduleTask {
  using tag = tag::LuaScheduleTask;
  // NOTE: not sure if I have to put optional here, but it maybe a good idea
  sol::protected_function onScheduleFunction;
  float interval = 1.f;
  float elapsed = 0;
};

} // namespace cmp
} // namespace pain
