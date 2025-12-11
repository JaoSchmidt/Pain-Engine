#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Scripting/Component.h"

class Player : public pain::NormalEntity
{
public:
  Player(pain::Scene &scene, pain::Texture &tex);
};
