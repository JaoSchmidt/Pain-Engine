#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Physics/MovementComponent.h"

namespace Stars
{
reg::Entity create(pain::Scene &scene, pain::TextureSheet &texSheet,
                   unsigned short id, glm::vec2 &pos);
reg::Entity create(pain::Scene &scene, pain::Texture &tex, unsigned short id,
                   glm::vec2 &pos);
}; // namespace Stars
