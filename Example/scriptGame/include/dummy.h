#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/GameObject.h"
#include "Scripting/Component.h"

class Dummy
    : public pain::GameObject<pain::TransformComponent, pain::SpriteComponent,
                              pain::LuaScriptComponent>
{
public:
  Dummy(pain::Scene *scene, const glm::vec2 &position, const glm::vec2 &size,
        const glm::vec4 &color, pain::Texture *ptexture, float tilingFactor);
};
