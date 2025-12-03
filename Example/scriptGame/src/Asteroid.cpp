// Asteroid.cpp
#include "Asteroid.h"
#include <pain.h>

Asteroid::Asteroid(pain::Scene &scene, pain::TextureSheet &texSheet, short id,
                   glm::vec2 pos, glm::vec2 vel, const float radius)
    : NormalEntity(scene)
{
  // assuming size of quad sprite = diameter
  const glm::vec2 diameter = glm::vec2(2.f * radius);
  createComponents(scene, pain::Transform2dComponent{pos},        //
                   pain::Movement2dComponent{vel},                //
                   pain::RotationComponent{},                     //
                   pain::SpriteComponent{texSheet, id, diameter}, //
                   pain::SpritelessComponent(radius),             //
                   pain::ColliderComponent::createCollider(radius));
}
Asteroid::Asteroid(pain::Scene &scene, pain::TextureSheet &texSheet, short id,
                   glm::vec2 pos, glm::vec2 vel, glm::vec2 size)
    : NormalEntity(scene)
{
  createComponents(scene, pain::Transform2dComponent{pos},    //
                   pain::Movement2dComponent{vel},            //
                   pain::RotationComponent{},                 //
                   pain::SpriteComponent{texSheet, id, size}, //
                   pain::SpritelessComponent(size),           //
                   pain::ColliderComponent::createCollider(size));
}
