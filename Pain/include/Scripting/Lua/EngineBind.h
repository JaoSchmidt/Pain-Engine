/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "Assets/ManagerMaterial.h"
#include <sol/state.hpp>
namespace pain::luabinder
{

void bindEngineMM(sol::state &lua, MaterialManager &mm);
void bindMaterial(sol::state &lua);
void bindEngine(sol::state &lua);

} // namespace pain::luabinder
