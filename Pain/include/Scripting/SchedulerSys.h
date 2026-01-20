/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file LuaSchedulerSys.h
 * @brief System responsible for executing scheduled Lua tasks.
 *
 * This system processes entities containing cmp::LuaScheduleTask and triggers
 * their scheduled callbacks when their accumulated time reaches the defined
 * interval.
 *
 * It acts as a lightweight scheduler layer for Lua-driven timed logic such as:
 *  - Delayed execution
 *  - Periodic tasks
 *  - Script-controlled timers
 *
 * @note As with any systems in the engine, system headers follow the naming
 * convention and end with the suffix "Sys.h".
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *
 * If the interface is removed, the engine will no longer invoke the associated
 * callback.
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

/**
 * @brief Executes scheduled Lua tasks based on elapsed time.
 *
 * LuaSchedulerSys iterates over all entities containing
 * cmp::LuaScheduleTask and accumulates elapsed time each frame. When the
 * elapsed time reaches the configured interval, the associated Lua callback
 * is executed.
 *
 * This system is typically used for:
 *  - Timed script execution
 *  - Repeating Lua jobs
 *  - Lightweight asynchronous gameplay logic
 *
 * The system only participates in the update phase.
 *
 * @note As with any system, the callback execution is only enabled because the
 * system inherits the corresponding interface:
 *  - IOnUpdate  → enables onUpdate() callbacks.
 *
 * @note The Tags list is validated at compile time to ensure that
 * cmp::LuaScheduleTask is registered in the component manager before this
 * system can be added to a scene.
 *
 * @see System
 * @see IOnUpdate
 * @see cmp::LuaScheduleTask
 */
struct LuaSchedulerSys : public System<WorldComponents>, IOnUpdate {
  /**
   * @brief Component tags required by this system.
   *
   * Declares that this system operates on entities containing
   * cmp::LuaScheduleTask. The engine validates these tags at compile time when
   * inserting the system into a scene.
   */
  using Tags = TypeList<cmp::LuaScheduleTask>;

  /** @brief Inherit base System constructors. */
  using System::System;

  /**
   * @brief Updates scheduled tasks and executes callbacks when due.
   *
   * Accumulates elapsed time for each scheduled task and invokes the Lua
   * callback once the configured interval has been reached.
   *
   * @param deltaTime Frame delta time.
   *
   * @note This method is invoked only because the system inherits from
   * IOnUpdate.
   */
  void onUpdate(DeltaTime deltaTime) override;
};

} // namespace Systems
} // namespace pain
