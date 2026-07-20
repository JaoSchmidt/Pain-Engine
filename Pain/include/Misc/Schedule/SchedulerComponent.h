/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file SchedulerComponent.h
 * @brief Component represent a lua callback to be called at a fixed interval.
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
/**
 * @brief Stores Schedule information of an entity
 *
 *
 * Defines the schedule interval to be called at the specified interval by the
 * schedule system
 */
struct LuaScheduleTask {
  using tag = tag::LuaScheduleTask;
  // NOTE: not sure if I have to put optional here, but it maybe a good idea
  sol::protected_function onScheduleFunction; /**< Calback Function */
  float interval = 1.f; /**< Local interval for callback call */
  float elapsed = 0;    /**< Elapsed time after last call */
};

} // namespace cmp
} // namespace pain
