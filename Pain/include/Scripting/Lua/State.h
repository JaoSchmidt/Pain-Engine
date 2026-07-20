/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ECS/WorldScene.h"
#include <sol/state.hpp>

namespace pain::luabinder
{

/**
 * Creates and initializes the main Lua state used by the engine.
 *
 * This function opens standard Lua libraries, overrides logging helpers,
 * and registers engine-facing types such as math vectors, colors,
 * components, and input bindings.
 *
 * The returned state represents the base scripting environment and is
 * created during Application initialization.
 */
sol::state createLuaState();

/** Registers the Lua scheduler API used for timed script execution. */
void addScheduler(sol::state &lua, Scene &worldScene);

/** Registers entity-related helpers exposed to Lua scripts. */
void addEntityFunctions(sol::state &lua, Scene &worldScene);

} // namespace pain::luabinder
