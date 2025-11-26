#include "Wall.h"
#include "Physics/Collision/GridManager.h"
#include <pain.h>

Wall::Wall(pain::Scene &scene, pain::GridManager &gm, const glm::vec2 &pos,
           const glm::vec2 &size)
    : NormalEntity(scene)
{
  glm::vec3 pos3 = glm::vec3(pos, 1.f);
  createComponents(
      scene, pain::TransformComponent{pos},                              //
      pain::SpritelessComponent{size, glm::vec4(0.8f, 0.2f, 0.1f, 1.f)}, //
      pain::ColliderComponent::createCollider({0.1f, 0.1f}));
  auto [cc, tc] =
      getComponents<pain::ColliderComponent, pain::TransformComponent>(scene);
  gm.insertStatic(cc, tc);
}
