#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"

namespace Stars
{
reg::Entity create(pain::Scene &scene, pain::TextureSheet &texSheet,
                   unsigned short id, glm::vec2 &pos);
reg::Entity create(pain::Scene &scene, pain::Texture &tex, unsigned short id,
                   glm::vec2 &pos);
}; // namespace Stars
