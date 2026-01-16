/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Asteroid.h
#pragma once

#include <glm/vec2.hpp>
#include <pain.h>

namespace Asteroid
{
reg::Entity create(pain::Scene &scene, pain::TextureSheet &texSheet,
                   unsigned short id, glm::vec2 pos, glm::vec2 vel,
                   glm::vec2 size);
reg::Entity create(pain::Scene &scene, pain::TextureSheet &texSheet,
                   unsigned short id, glm::vec2 pos, glm::vec2 vel,
                   const float radius);
} // namespace Asteroid
