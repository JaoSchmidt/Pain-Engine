#pragma once

#include "Core.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"

class Wall : public pain::NormalEntity<pain::Transform2dComponent, //
                                       pain::SpritelessComponent,  //
                                       pain::ColliderComponent>    //
{
public:
  Wall(pain::Scene &scene, const glm::vec2 &pos, const glm::vec2 &size);
};
