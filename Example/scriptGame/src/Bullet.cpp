#include "Bullet.h"
#include "ECS/Components/Particle.h"
#include "Scripting/Component.h"
#include <pain.h>

Bullet::Bullet(pain::Scene *scene) : NormalEntity(*scene)
{
  createComponents(*scene, pain::TransformComponent{}, //
                   pain::SpriteComponent{},            //
                   pain::MovementComponent{});
}
