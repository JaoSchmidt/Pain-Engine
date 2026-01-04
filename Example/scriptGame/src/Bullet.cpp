#include "Bullet.h"
#include "ECS/Components/Particle.h"
#include "Scripting/Component.h"
#include <pain.h>

reg::Entity Bullet::create(pain::Scene &scene)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, pain::Transform2dComponent{}, //
                         pain::SpriteComponent{},              //
                         pain::Movement2dComponent{});
  return entity;
}
