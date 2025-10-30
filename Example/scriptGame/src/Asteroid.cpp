#include "Asteroid.h"
#include "Physics/Collision/GridManager.h"
#include <pain.h>

Asteroid::Asteroid(pain::Scene &scene, pain::GridManager &gm,
                   pain::TextureSheet &texSheet, short id, glm::vec2 &pos,
                   glm::vec2 &vel)
    : NormalEntity(scene)
{
  glm::vec3 pos3 = glm::vec3(pos, 1.f);
  createComponents(scene, pain::TransformComponent{pos},         //
                   pain::MovementComponent{glm::vec3(vel, 1.f)}, //
                   pain::RotationComponent{},
                   pain::SpriteComponent{texSheet, id}, //
                   pain::ColliderComponent::createCollider({0.1f, 0.1f}));
}
