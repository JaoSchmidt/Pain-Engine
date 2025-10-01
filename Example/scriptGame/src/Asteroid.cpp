#include "Asteroid.h"
#include <pain.h>

Asteroid::Asteroid(pain::Scene *scene) : NormalEntity(*scene)
{
  createComponents(*scene, pain::TransformComponent{}, //
                   pain::SpriteComponent{},            //
                   pain::MovementComponent{});
}
