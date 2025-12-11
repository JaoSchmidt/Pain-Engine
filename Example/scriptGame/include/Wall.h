#pragma once

#include "Core.h"
#include <pain.h>

namespace Wall
{
reg::Entity create(pain::Scene &scene, const glm::vec2 &pos,
                   const glm::vec2 &size);
};
