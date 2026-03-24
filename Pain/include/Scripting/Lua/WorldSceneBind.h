/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Assets/ManagerMaterial.h"
#include "CoreFiles/AppInitConfig.h"
#include "ECS/WorldScene.h"

namespace pain::luabinder
{

void bindWorldComponents(pain::Scene &scene, sol::state &lua,
                         pain::MaterialManager &mm, const AppInit &initConfig);

/**
 * Registers component access helpers for LuaScriptComponent instances.
 *
 * Exposes functions such as get_position(), get_sprite(), and get_movement()
 * to Lua, allowing scripts to safely access ECS components owned by the
 * bound entity.
 *
 * Component access is validated at runtime and returns nil when the
 * component does not exist.
 *
 * This function is called during Application initialization.
 *
 * @tparam AbstractScene Scene type providing ECS access.
 * @param solstate Lua state to register bindings into.
 * @param worldScene Scene instance used for component lookup.
 */
void addLuaComponentFunctions(sol::state &lua, Scene &worldScene);

void printState(sol::state &lua);

} // namespace pain::luabinder
