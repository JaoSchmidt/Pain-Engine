#pragma once

#include "CoreRender/Texture.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"

class Bullet : public pain::NormalEntity<pain::TransformComponent, //
                                         pain::SpriteComponent,    //
                                         pain::MovementComponent>  //
{
public:
  Bullet(pain::Scene *scene);
};
