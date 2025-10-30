#pragma once

#include "Core.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Physics/Collision/GridManager.h"

class Asteroid : public pain::NormalEntity<pain::TransformComponent, //
                                           pain::MovementComponent,  //
                                           pain::RotationComponent,  //
                                           pain::SpriteComponent,    //
                                           pain::ColliderComponent>  //
{
public:
  Asteroid(pain::Scene &scene, pain::GridManager &gm,
           pain::TextureSheet &texSheet, short id, glm::vec2 &pos,
           glm::vec2 &vel);
};
