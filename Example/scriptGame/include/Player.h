#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Scriptable.h"
#include "Scripting/Component.h"

class Player : public pain::NormalEntity<pain::Transform2dComponent,       //
                                         pain::SpriteComponent,            //
                                         pain::Movement2dComponent,        //
                                         pain::ParticleSprayComponent,     //
                                         pain::OrthoCameraComponent,       //
                                         pain::CollisionCallbackComponent, //
                                         pain::LuaScriptComponent>
{
public:
  Player(pain::Scene &scene, pain::Texture &tex);
};
