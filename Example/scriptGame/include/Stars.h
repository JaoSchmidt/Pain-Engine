#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"

class Stars : public pain::NormalEntity //

{
public:
  Stars(pain::Scene &scene, pain::TextureSheet &texSheet, short id,
        glm::vec2 &pos);
  Stars(pain::Scene &scene, pain::Texture &tex, short id, glm::vec2 &pos);
};
