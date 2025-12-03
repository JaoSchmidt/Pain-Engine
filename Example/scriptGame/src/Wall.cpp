#include "Wall.h"
#include <pain.h>

Wall::Wall(pain::Scene &scene, const glm::vec2 &pos, const glm::vec2 &size)
    : NormalEntity(scene)
{
  createComponents(
      scene, pain::Transform2dComponent{pos},                            //
      pain::SpritelessComponent{size, glm::vec4(0.8f, 0.2f, 0.1f, 1.f)}, //
      pain::ColliderComponent::createCollider(size));
}
