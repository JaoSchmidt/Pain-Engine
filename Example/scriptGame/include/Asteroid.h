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
