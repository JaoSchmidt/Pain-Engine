#pragma once

#include "Core.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"

class Asteroid : public pain::NormalEntity<pain::TransformComponent, //
                                           pain::SpriteComponent,    //
                                           pain::MovementComponent>  //
{
public:
  Asteroid(pain::Scene *scene);
};
