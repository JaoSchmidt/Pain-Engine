// Asteroid.cpp
#include "Asteroid.h"
#include <pain.h>

reg::Entity Asteroid::create(pain::Scene &scene, pain::TextureSheet &texSheet,
                             unsigned short id, glm::vec2 pos, glm::vec2 vel,
                             const float radius)
{
  // assuming size of quad sprite = diameter
  const glm::vec2 diameter = glm::vec2(2.f * radius);
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, pain::Transform2dComponent{pos},       //
                         pain::Movement2dComponent{vel},                //
                         pain::RotationComponent{},                     //
                         pain::SpriteComponent{texSheet, id, diameter}, //
                         pain::SpritelessComponent(radius),             //
                         pain::SAPCollider::createCollider(radius));
  return entity;
}
reg::Entity Asteroid::create(pain::Scene &scene, pain::TextureSheet &texSheet,
                             unsigned short id, glm::vec2 pos, glm::vec2 vel,
                             glm::vec2 size)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, pain::Transform2dComponent{pos},   //
                         pain::Movement2dComponent{vel},            //
                         pain::RotationComponent{},                 //
                         pain::SpriteComponent{texSheet, id, size}, //
                         pain::SpritelessComponent(size),           //
                         pain::SAPCollider::createCollider(size));
  return entity;
}
