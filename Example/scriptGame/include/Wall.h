#pragma once

#include "Core.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Physics/Collision/GridManager.h"

class Wall : public pain::NormalEntity<pain::TransformComponent,  //
                                       pain::SpritelessComponent, //
                                       pain::ColliderComponent>   //
{
public:
  Wall(pain::Scene &scene, pain::GridManager &gm, const glm::vec2 &pos,
       const glm::vec2 &size);
};
