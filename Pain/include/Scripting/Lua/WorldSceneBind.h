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

void printState(sol::state &lua);

} // namespace pain::luabinder
