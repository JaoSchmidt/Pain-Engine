/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "ECS/WorldScene.h"
#include <sol/state.hpp>
namespace pain
{

sol::state createLuaState();
template <typename AbstractScene>
void addComponentFunctions(sol::state &solstate, AbstractScene &worldScene);
void addScheduler(sol::state &lua, Scene &worldScene);
void addEntityFunctions(sol::state &lua, Scene &worldScene);
} // namespace pain
