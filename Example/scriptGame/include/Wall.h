/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"
#include <pain.h>

namespace Wall
{
reg::Entity create(pain::Scene &scene, const glm::vec2 &pos,
                   const glm::vec2 &size);
};
